
#include "echange.h"
int envoyer_message(Joueur *joueur, char *message)
{
    int n;
    n = write(*joueur->socket, message, strlen(message));
    if (n < 0)
    {
        printf("ERROR");
        perror("ERROR writing to socket");
        exit(1);
    }
    return n;
}

void envoyer_erreur(Joueur *joueur)
{
    int n;
    char message[MAX_MESSAGE_SIZE];
    snprintf(message, sizeof(message), "/message #server #%s Erreur : Le joueur n'existe pas ou n'est pas connecté.", joueur->nom);
    envoyer_message(joueur, message);
}