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
        char *string_plateau = malloc(sizeof(char) * 2048);
        string_plateau = afficherPlateau(jeu);
        envoyer_plateau(jeu, string_plateau);
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
    printf("\033[31m"
           "Le nombre généré est %d\n", // Remplacement par %d pour un entier
           nombreAleatoire);

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
    if (jeu == NULL)
    {
        perror("Erreur d'allocation mémoire pour le jeu");
        exit(EXIT_FAILURE);
    }

    jeu->joueur1 = demandeur;
    jeu->joueur2 = joueur;

    if (jeu->joueur1 == NULL || jeu->joueur2 == NULL)
    {
        fprintf(stderr, "Les joueurs ne sont pas correctement initialisés.\n");
        free(jeu);
        exit(EXIT_FAILURE);
    }

    initialiserPlateau(jeu);

    if (lobby->nbJeux >= MAX_PARTIES)
    {
        fprintf(stderr, "Le lobby est plein, impossible de démarrer un nouveau jeu.\n");
        free(jeu);
        exit(EXIT_FAILURE);
    }

    joueur->idPartie = lobby->nbJeux;
    demandeur->idPartie = lobby->nbJeux;

    lobby->jeux[lobby->nbJeux] = jeu;
    lobby->nbJeux++;
    srand(time(NULL));
    qui_commence(jeu);
}
