#include "partieManager.h"
void qui_commence(Jeu *jeu)
{
    int nombreAleatoire = (rand() % 2) + 1;
    printf("\033[31m"
           "Le nombre généré est %d\n\033[0m", // Remplacement par %d pour un entier
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
void calculerElo(Joueur *joueur1, Joueur *joueur2, int resultatA)
{
    double E1 = 1.0 / (1.0 + pow(10, (joueur2->elo - joueur1->elo) / 400.0));
    double E2 = 1.0 / (1.0 + pow(10, (joueur1->elo - joueur2->elo) / 400.0));

    joueur1->elo = joueur1->elo + FACTEUR_DAJUSTEMENT_ELO * (resultatA - E1);
    joueur2->elo = joueur2->elo + FACTEUR_DAJUSTEMENT_ELO * ((1 - resultatA) - E2);
}
