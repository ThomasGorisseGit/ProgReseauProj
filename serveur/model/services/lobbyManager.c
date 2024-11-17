#include "lobbyManager.h"

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
    joueur->status = INIT;
    joueur->idPartie = -1;
    joueur->idJoueur = lobby->nbJoueurs;

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

    free(jeu);
    jeu = NULL;
}