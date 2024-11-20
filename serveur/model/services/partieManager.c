#include "partieManager.h"

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
