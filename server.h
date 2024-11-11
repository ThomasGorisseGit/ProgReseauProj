#ifndef SERVER_H
#define SERVER_H
#define MAX_NAME_SIZE 48
#define MAX_MESSAGE_SIZE 1024
#define MAX_PLAYER 10

typedef struct
{
    Joueur **joueurs;
    int nbJoueurs;
} Lobby;

#endif // SERVER_H