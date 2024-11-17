#ifndef SERVER_H
#define SERVER_H
#define MAX_NAME_SIZE 48
#define MAX_MESSAGE_SIZE 2048
#define MAX_PLAYER 10
#define MAX_PARTIES 5

typedef struct
{
    Joueur **joueurs;
    int nbJoueurs;
    Jeu **jeux;
    int nbJeux;
} Lobby;

#endif // SERVER_H