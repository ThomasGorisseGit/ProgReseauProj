#include "communication.h"
void envoyerATousDansLobby(Lobby *lobby, char *message)
{
    for (int i = 0; i < lobby->nbJoueurs; i++)
    {
        if (lobby->joueurs[i]->status == LOBBY)
        {
            envoyer(lobby->joueurs[i], message);
        }
    }
}

int envoyer(Joueur *joueur, char *message)
{
    int n;
    n = write(*joueur->socket, message, strlen(message));
    if (n < 0)
    {
        perror("ERROR writing to socket");
        exit(1);
    }
    return n;
}