#include "partieManager.h"

void gerer_coup(Jeu *jeu, Joueur *joueur, int case_depart)
{
    if (joueur->idPartie == -1)
    {
        envoyer_erreur(joueur);
        return;
    }
    if (joueur->status != PARTIE)
    {
        envoyer_erreur(joueur);
        return;
    }
    if (jeu->current->nom != joueur->nom)
    {
        envoyer_erreur(joueur);
        return;
    }
    int coups_valide = jouerCoup(jeu, case_depart);
    if (coups_valide == -1)
    {
        // Le coups est invalide, on redemande a l'utilisateur
        envoyer_erreur(joueur);
        demander_case_depart(joueur);
        return;
    }
    if (coups_valide == 0)
    {
        // égalité
        envoyer_egalite(jeu);
        fin_partie(jeu);
    }
    if (jeu->vainqueur != NULL)
    {
        // On a un vainqueur
        envoyer_gagnant(jeu);
        fin_partie(jeu);
    }

    if (coups_valide == 1)
    {
        // on continue la partie :
        envoyer_plateau(jeu);
        if (jeu->current == jeu->joueur1)
        {
            jeu->current = jeu->joueur2;
        }
        else
        {
            jeu->current = jeu->joueur1;
        }
        envoyer_le_joueur_courant(jeu);
        demander_case_depart(jeu->current);
    }
}
void qui_commence(Jeu *jeu)
{
    int nombreAleatoire = (rand() % 2) + 1;
    if (nombreAleatoire == 1)
    {
        jeu->current = jeu->joueur1;
    }
    else
    {
        jeu->current = jeu->joueur2;
    }
}
void initialiser_jeu(Lobby *lobby, Joueur *demandeur, Joueur *joueur)
{
    demandeur->status = PARTIE;
    joueur->status = PARTIE;

    // Initialiser la partie
    Jeu *jeu = malloc(sizeof(Jeu));
    // vérification de l'existance du jeu
    if (jeu == NULL)
    {
        perror("Erreur d'allocation mémoire pour le jeu");
        exit(1);
    }
    jeu->joueur1 = demandeur;
    jeu->joueur2 = joueur;
    jeu->current = demandeur;
    jeu->joueur1 = demandeur;
    jeu->joueur2 = joueur;

    if (jeu->joueur1 == NULL || jeu->joueur2 == NULL || jeu->current == NULL)
    {
        fprintf(stderr, "Les joueurs ne sont pas correctement initialisés.\n");
        free(jeu); // Libération de la mémoire pour éviter une fuite
        exit(1);
    }

    joueur->idPartie = lobby->nbJeux;
    demandeur->idPartie = lobby->nbJeux;

    lobby->jeux[lobby->nbJeux] = jeu;
    lobby->nbJeux++;

    qui_commence(jeu);
}