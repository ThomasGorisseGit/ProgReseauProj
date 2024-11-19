#include "partieManager.h"

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
        printf("Erreur d'allocation mémoire pour le jeu");
        exit(1);
    }
    jeu->joueur1 = demandeur;
    jeu->joueur2 = joueur;

    printf("Joueurs : %s et %s\n", jeu->joueur1->nom, jeu->joueur2->nom);

    if (jeu->joueur1 == NULL || jeu->joueur2 == NULL)
    {
        fprintf(stderr, "Les joueurs ne sont pas correctement initialisés.\n");
        free(jeu); // Libération de la mémoire pour éviter une fuite
        exit(1);
    }
    initialiserPlateau(jeu);
    joueur->idPartie = lobby->nbJeux;
    demandeur->idPartie = lobby->nbJeux;

    lobby->jeux[lobby->nbJeux] = jeu;
    lobby->nbJeux++;

    qui_commence(jeu);
}