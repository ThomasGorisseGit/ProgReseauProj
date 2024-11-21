
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
    snprintf(message, sizeof(message), "/case #server #%s Entrez la case de départ ", joueur->nom);
    envoyer_message(joueur, message);
}
void envoyer_gagnant(Jeu *jeu)
{
    char message[MAX_MESSAGE_SIZE];
    snprintf(message, sizeof(message), "/message #server #%s %s a gagné la partie", jeu->vainqueur->nom, jeu->vainqueur->nom);
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

    char buffer[MAX_BODY_SIZE];
    int pos = 0;
    pos += snprintf(buffer + pos, sizeof(buffer) - pos, "Joueur 2 : %s\nScore : %d\n\n",
                    jeu->joueur2->nom, jeu->joueur2->score);
    pos += snprintf(buffer + pos, sizeof(buffer) - pos, "  ");
    for (int i = 6; i < 12; i++)
    {
        pos += snprintf(buffer + pos, sizeof(buffer) - pos, " %2d  ", i);
    }
    pos += snprintf(buffer + pos, sizeof(buffer) - pos, "\n");

    // Rangée supérieure : graines
    pos += snprintf(buffer + pos, sizeof(buffer) - pos, "  ");
    for (int i = 6; i < 12; i++)
    {
        pos += snprintf(buffer + pos, sizeof(buffer) - pos, " %2d  ", jeu->plateau[i].nbGraines);
    }
    pos += snprintf(buffer + pos, sizeof(buffer) - pos, "\n");

    // Ligne de séparation
    pos += snprintf(buffer + pos, sizeof(buffer) - pos, "  ----------------------------\n");

    // Rangée inférieure : indices (0 à 5)
    pos += snprintf(buffer + pos, sizeof(buffer) - pos, "  ");
    for (int i = 5; i >= 0; i--)
    {
        pos += snprintf(buffer + pos, sizeof(buffer) - pos, " %2d  ", i);
    }
    pos += snprintf(buffer + pos, sizeof(buffer) - pos, "\n");

    // Rangée inférieure : graines
    pos += snprintf(buffer + pos, sizeof(buffer) - pos, "  ");
    for (int i = 5; i >= 0; i--)
    {
        pos += snprintf(buffer + pos, sizeof(buffer) - pos, " %2d  ", jeu->plateau[i].nbGraines);
    }
    pos += snprintf(buffer + pos, sizeof(buffer) - pos, "\n\n");

    // Informations sur le joueur 1
    pos += snprintf(buffer + pos, sizeof(buffer) - pos, "Joueur 1 : %s\nScore : %d\n",
                    jeu->joueur1->nom, jeu->joueur1->score);

    char message[MAX_MESSAGE_SIZE];
    snprintf(message, sizeof(message), "/message #server #%s %s", jeu->joueur1->nom, buffer);
    envoyer_message(jeu->joueur1, message);
    envoyer_message(jeu->joueur2, message);
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
        if (lobby->joueurs[i]->status == LOBBY)
        {
            message = strcat(message, "Lobby");
        }
        else if (lobby->joueurs[i]->status == INIT)
        {
            message = strcat(message, "Initialisation");
        }
        else
        {
            message = strcat(message, "En partie");
        }
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
    snprintf(message, sizeof(message), "/rejoindre #server #%s Un nouveau joueur a rejoint le lobby : %s\n", joueur->nom, joueur->nom);
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

void envoyer_bio(Joueur *joueur)
{
    char message[MAX_MESSAGE_SIZE];
    snprintf(message, sizeof(message), "/modifierBio #server #%s Votre bio a été modifiée : %s", joueur->nom, joueur->bio);
    envoyer_message(joueur, message);
}
void envoyer_elo_joueurs(Jeu *jeu)
{
    char message[MAX_MESSAGE_SIZE];
    snprintf(message, sizeof(message), "/message #server #%s Votre ELO est de %f", jeu->joueur1->nom, jeu->joueur1->elo);
    envoyer_message(jeu->joueur1, message);
    snprintf(message, sizeof(message), "/message #server #%s Votre ELO est de %f", jeu->joueur2->nom, jeu->joueur2->elo);
    envoyer_message(jeu->joueur2, message);
}
void envoyer_classement(Lobby *lobby, Joueur *joueur)
{
    int totalLength = strlen("Classement des joueurs :\n");
    for (int i = 0; i < lobby->nbJoueurs; i++)
    {
        totalLength += strlen(lobby->joueurs[i]->nom) + 1; // +1 pour le saut de ligne
    }
    char *message = malloc(totalLength * sizeof(char));

    strcpy(message, "Pseudo\t\t\t | \t\tELO\n");
    message = strcat(message, "-------------------------|-------------------------\n");
    for (int i = 0; i < lobby->nbJoueurs; i++)
    {
        message = strcat(message, lobby->joueurs[i]->nom);
        message = strcat(message, "\t\t\t | \t\t");
        char elo[10];
        snprintf(elo, sizeof(elo), "%f", lobby->joueurs[i]->elo);
        message = strcat(message, elo);
        message = strcat(message, "\n");
    }

    char messageToSend[MAX_MESSAGE_SIZE];
    snprintf(messageToSend, sizeof(messageToSend), "/classement #server #%s Classement des joueurs : \n%s", joueur->nom, message);
    envoyer_message(joueur, messageToSend);
}
void envoyer_nom_valide(Joueur *joueur)
{
    char message[MAX_MESSAGE_SIZE];
    snprintf(message, sizeof(message), "/nomValide #server #%s Pseudo Valide !", joueur->nom);
    envoyer_message(joueur, message);
}

void envoyer_nom_invalide(Joueur *joueur)
{
    char message[MAX_MESSAGE_SIZE];
    snprintf(message, sizeof(message), "/nomInvalide #server #%s Pseudo Invalide, veuillez en choisir un autre !", joueur->nom);
    envoyer_message(joueur, message);
}