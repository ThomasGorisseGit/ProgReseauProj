
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
    char message[MAX_MESSAGE_SIZE];
    snprintf(message, sizeof(message), "/message #server #%s Erreur : Le joueur n'existe pas ou n'est pas connecté.", joueur->nom);
    return envoyer_message(joueur, message);
}

void envoyer_a_tous(Lobby *lobby, char *message)
{
    for (int i = 0; i < lobby->nbJoueurs; i++)
    {
        if (lobby->joueurs[i]->status == LOBBY)
        {
            printf("Envoie vers %s\n  %s", lobby->joueurs[i]->nom, message);
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
    snprintf(message, sizeof(message), "/message #server #%s Entrez la case de départ ", joueur->nom);
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
    envoyer_message(jeu->joueur1, "plateau");
    envoyer_message(jeu->joueur2, "plateau");
}

void envoyer_liste_joueurs(Lobby *lobby, Joueur *joueur)
{
    int totalLength = strlen("Liste des joueurs connectés :\n");
    for (int i = 0; i < lobby->nbJoueurs; i++)
    {
        totalLength += strlen(lobby->joueurs[i]->nom) + 1; // +1 pour le saut de ligne
    }
    char *message = malloc(totalLength * sizeof(char));

    strcpy(message, "Pseudo\t\t | \tStatus\n");
    for (int i = 0; i < lobby->nbJoueurs; i++)
    {
        message = strcat(message, lobby->joueurs[i]->nom);
        message = strcat(message, "\t\t | \t");
        message = strcat(message, lobby->joueurs[i]->status == LOBBY ? "Lobby" : "En partie");
        message = strcat(message, "\n");
    }

    char messageToSend[MAX_MESSAGE_SIZE];
    snprintf(messageToSend, sizeof(messageToSend), "/message #server #%s Liste des joueurs connectés : \n%s", joueur->nom, message);
    envoyer_message(joueur, messageToSend);
}

void envoyer_defi(Joueur *joueur, Joueur *defie)
{
    char message[MAX_MESSAGE_SIZE];
    snprintf(message, sizeof(message), "/defier #%s #%s Le joueur %s veut vous défier. Pour accepter, tapez 1 ; pour refuser, tapez 0.", joueur->nom, defie->nom, joueur->nom);
    envoyer_message(defie, message);
}

void envoyer_rejoindre(Joueur *joueur, Lobby *lobby)
{
    char message[MAX_MESSAGE_SIZE];
    snprintf(message, sizeof(message), "/joining #server #%s Un nouveau joueur a rejoint le lobby : %s\n", joueur->nom, joueur->nom);
    envoyer_a_tous(lobby, message);
}
void envoyer_decliner_defi(Joueur *joueur, Joueur *demandeur)
{
    char message[MAX_MESSAGE_SIZE];
    snprintf(message, sizeof(message), "/message #server #%s %s a refusé votre défi.", demandeur->nom, joueur->nom);
    envoyer_message(demandeur, message);
}
void envoyer_accepter_defi(Joueur *joueur, Joueur *demandeur)
{
    char message[MAX_MESSAGE_SIZE];
    snprintf(message, sizeof(message), "/message #server #%s %s a accepté votre défi.", demandeur->nom, joueur->nom);
    envoyer_message(demandeur, message);
}