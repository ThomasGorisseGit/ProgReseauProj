#include "communication.h"
void envoyerATousDansLobby(Lobby *lobby, char *message)
{
    for (int i = 0; i < lobby->nbJoueurs; i++)
    {
        if (lobby->joueurs[i]->status == LOBBY)
        {
            printf("Envoi à %s\n", lobby->joueurs[i]->nom);
            envoyer(lobby->joueurs[i], message);
        }
    }
}

int envoyer(Joueur *joueur, char *message)
{
    int n;
    n = write(*joueur->socket, message, strlen(message));
    printf("Message envoyé:%s\n", message);
    if (n < 0)
    {
        printf("ERROR");
        perror("ERROR writing to socket");
        exit(1);
    }
    return n;
}