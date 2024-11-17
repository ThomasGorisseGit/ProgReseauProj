
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

int envoyer_erreur(Joueur *joueur)
{
    int n;
    char message[MAX_MESSAGE_SIZE];
    snprintf(message, sizeof(message), "/message #server #%s Erreur : Le joueur n'existe pas ou n'est pas connecté.", joueur->nom);
    envoyer_message(joueur, message);
}

int envoyer_a_tous(Lobby *lobby, char *message)
{
    for (int i = 0; i < lobby->nbJoueurs; i++)
    {
        if (lobby->joueurs[i]->status == LOBBY)
        {
            envoyer_message(lobby->joueurs[i], message);
        }
    }
}

void envoyer_le_joueur_courant(Jeu *jeu)
{
    char message[MAX_MESSAGE_SIZE];
    snprintf(message, sizeof(message), "/message #server #%s Le joueur %s va commencer", jeu->current->nom, jeu->current->nom);
    envoyer_message(jeu->joueur1, message);
    envoyer_message(jeu->joueur2, message);
}

void demander_case_depart(Joueur *joueur)
{
    char message[MAX_MESSAGE_SIZE];
    snprintf(message, sizeof(message), "/message #server #%s Entrez la case de départ : ", joueur->nom);
    envoyer_message(joueur, message);
}
void envoyer_gagnant(Jeu *jeu)
{
    char message[MAX_MESSAGE_SIZE];
    snprintf(message, sizeof(message), "/message #server #%s a gagné", jeu->vainqueur->nom);
    envoyer_message(jeu->joueur1, message);
    envoyer_message(jeu->joueur2, message);
}
void envoyer_egalite(Jeu *jeu)
{
    char message[MAX_MESSAGE_SIZE];
    snprintf(message, sizeof(message), "/message #server #Egalité");
    envoyer_message(jeu->joueur1, message);
    envoyer_message(jeu->joueur2, message);
}
void envoyer_plateau(Jeu *jeu)
{
    // Il faut l'implanter
}
