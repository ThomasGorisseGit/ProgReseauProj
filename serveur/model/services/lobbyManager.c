#include "lobbyManager.h"
#include <stdio.h>
#include <stdlib.h>
Lobby *initialiserLobby()
{

    Lobby *lobby = malloc(sizeof(Lobby));

    lobby->joueurs = malloc(MAX_PLAYER * sizeof(Joueur *));
    lobby->nbJoueurs = 0;
    lobby->jeux = malloc(MAX_PARTIES * sizeof(Jeu *));
    lobby->nbJeux = 0;

    return lobby;
}

void handle_connection(int sockfd, fd_set *readfds, int *max_fd, Lobby *lobby)
{
    struct sockaddr_in cli_addr;
    socklen_t clilen = sizeof(cli_addr);
    int *newsockfd = malloc(sizeof(int));

    *newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    if (*newsockfd < 0)
    {
        perror("ERROR on accept");
        free(newsockfd);
        return;
    }
    printf("Nouvelle connexion acceptée\n");

    Joueur *joueur = malloc(sizeof(Joueur));
    joueur->socket = newsockfd;

    joueur->nom = malloc(MAX_NAME_SIZE * sizeof(char));
    strcpy(joueur->nom, "Anonyme");

    joueur->bio = malloc(MAX_NAME_SIZE * sizeof(char));
    strcpy(joueur->bio, "Ce joueurs n'a pas de bio");

    joueur->status = INIT;
    joueur->type = HUMAIN;
    joueur->idPartie = -1;
    joueur->idJoueur = lobby->nbJoueurs;
    joueur->elo = 1000.0;
    joueur->score = 0;

    // Ajouter le nouveau joueur au lobby
    lobby->joueurs[lobby->nbJoueurs] = joueur;
    lobby->nbJoueurs++;

    // Ajouter le socket du joueur à l'ensemble des descripteurs à surveiller
    FD_SET(*newsockfd, readfds);
    if (*newsockfd > *max_fd)
        *max_fd = *newsockfd;
}

void fin_partie(Jeu *jeu)
{
    jeu->joueur1->status = LOBBY;
    jeu->joueur2->status = LOBBY;

    jeu->joueur1->idPartie = -1;
    jeu->joueur2->idPartie = -1;
    // free(jeu->plateau);
    // free(jeu);
}

Joueur *trouver_joueur(Lobby *lobby, const char *nom)
{
    for (int i = 0; i < lobby->nbJoueurs; i++)
    {
        if (strcmp(lobby->joueurs[i]->nom, nom) == 0) // Comparaison des noms
        {
            return lobby->joueurs[i]; // Retourne le joueur trouvé
        }
    }
    return NULL; // Retourne NULL si le joueur n'est pas trouvé
}

Joueur *defier_joueur(Lobby *lobby, char *pseudo)
{
    for (int i = 0; i < lobby->nbJoueurs; i++)
    {
        Joueur *joueur = lobby->joueurs[i];

        // Vérifie si le pseudo correspond
        if (strcmp(joueur->nom, pseudo) == 0)
        {
            // Vérifie si le joueur est dans le lobby et disponible
            if (joueur->status == LOBBY)
            {
                return joueur; // Retourne le pointeur vers le joueur disponible
            }
            else
            {
                return NULL; // Le joueur est en partie
            }
        }
    }
    return NULL;
}
