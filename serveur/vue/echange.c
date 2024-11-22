
#include "echange.h"
int envoyer_message(Joueur *joueur, char *message)
{
    if (joueur->socket == NULL)
    {
        return 0;
    }
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

int envoyer_erreur(Joueur *joueur, char erreur[MAX_BODY_SIZE])
{
    char message[MAX_MESSAGE_SIZE];
    snprintf(message, sizeof(message), "/message #server #%s %sErreur :%s\n %s%s%s", joueur->nom, COLOR_RED_BOLD, COLOR_RESET, COLOR_RED, erreur, COLOR_RESET);
    return envoyer_message(joueur, message);
}

void envoyer_a_tous(Lobby *lobby, char *message)
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
    snprintf(message, sizeof(message), "/message #server #%s Le joueur %s%s%s va commencer", jeu->current->nom, COLOR_BLUE, jeu->current->nom, COLOR_RESET);
    envoyer_message(jeu->joueur1, message);
    envoyer_message(jeu->joueur2, message);
}

void demander_case_depart(Joueur *joueur)
{
    char message[MAX_MESSAGE_SIZE];
    snprintf(message, sizeof(message), "/case #server #%s %sEntrez la case de départ %s", joueur->nom, COLOR_GREEN, COLOR_RESET);
    envoyer_message(joueur, message);
}
void envoyer_gagnant(Jeu *jeu)
{
    char message[MAX_MESSAGE_SIZE];
    snprintf(message, sizeof(message), "/message #server #%s %s%s%s a gagné la partie", jeu->vainqueur->nom, COLOR_GREEN, jeu->vainqueur->nom, COLOR_RESET);
    envoyer_message(jeu->joueur1, message);
    envoyer_message(jeu->joueur2, message);
}
void envoyer_egalite(Jeu *jeu)
{
    char message[MAX_MESSAGE_SIZE];
    snprintf(message, sizeof(message), "/message #server #%s %sMatch nul%s", jeu->joueur1->nom, COLOR_BLUE, COLOR_RESET);
    envoyer_message(jeu->joueur1, message);

    snprintf(message, sizeof(message), "/message #server #%s %sMatch nul%s", jeu->joueur2->nom, COLOR_BLUE, COLOR_RESET);
    envoyer_message(jeu->joueur2, message);
}
void envoyer_plateau(Jeu *jeu)
{
    char buffer_joueur1[MAX_BODY_SIZE];
    char buffer_joueur2[MAX_BODY_SIZE];

    // Plateau pour le joueur 1
    int pos1 = 0;
    pos1 += snprintf(buffer_joueur1 + pos1, sizeof(buffer_joueur1) - pos1, "%sJoueur 2 :%s %s\n%sScore :%s %d\n",
                     COLOR_GREEN, COLOR_RESET,
                     jeu->joueur2->nom,
                     COLOR_GREEN, COLOR_RESET,
                     jeu->joueur2->score);

    pos1 += snprintf(buffer_joueur1 + pos1, sizeof(buffer_joueur1) - pos1, "  ");
    for (int i = 11; i >= 6; i--)
    {
        pos1 += snprintf(buffer_joueur1 + pos1, sizeof(buffer_joueur1) - pos1, " %2d  ", i);
    }
    pos1 += snprintf(buffer_joueur1 + pos1, sizeof(buffer_joueur1) - pos1, "\n");

    pos1 += snprintf(buffer_joueur1 + pos1, sizeof(buffer_joueur1) - pos1, "  ");
    for (int i = 11; i >= 6; i--)
    {
        pos1 += snprintf(buffer_joueur1 + pos1, sizeof(buffer_joueur1) - pos1, " %2d  ", jeu->plateau[i].nbGraines);
    }
    pos1 += snprintf(buffer_joueur1 + pos1, sizeof(buffer_joueur1) - pos1, "\n");

    pos1 += snprintf(buffer_joueur1 + pos1, sizeof(buffer_joueur1) - pos1, "  ----------------------------\n");

    pos1 += snprintf(buffer_joueur1 + pos1, sizeof(buffer_joueur1) - pos1, "  ");
    for (int i = 0; i < 6; i++)
    {
        pos1 += snprintf(buffer_joueur1 + pos1, sizeof(buffer_joueur1) - pos1, " %2d  ", i);
    }
    pos1 += snprintf(buffer_joueur1 + pos1, sizeof(buffer_joueur1) - pos1, "\n");

    pos1 += snprintf(buffer_joueur1 + pos1, sizeof(buffer_joueur1) - pos1, "  ");
    for (int i = 0; i < 6; i++)
    {
        pos1 += snprintf(buffer_joueur1 + pos1, sizeof(buffer_joueur1) - pos1, " %2d  ", jeu->plateau[i].nbGraines);
    }
    pos1 += snprintf(buffer_joueur1 + pos1, sizeof(buffer_joueur1) - pos1, "\n\n");
    pos1 += snprintf(buffer_joueur1 + pos1, sizeof(buffer_joueur1) - pos1, "%sJoueur 1 :%s %s\n%sScore :%s %d\n\n",
                     COLOR_GREEN, COLOR_RESET,
                     jeu->joueur1->nom,
                     COLOR_GREEN, COLOR_RESET,
                     jeu->joueur1->score);

    // Plateau pour le joueur 2
    int pos2 = 0;

    pos2 += snprintf(buffer_joueur2 + pos2, sizeof(buffer_joueur2) - pos2, "%sJoueur 1 :%s %s\n%sScore :%s %d\n",
                     COLOR_GREEN, COLOR_RESET,
                     jeu->joueur1->nom,
                     COLOR_GREEN, COLOR_RESET,
                     jeu->joueur1->score);
    pos2 += snprintf(buffer_joueur2 + pos2, sizeof(buffer_joueur2) - pos2, "  ");
    for (int i = 5; i >= 0; i--)
    {
        pos2 += snprintf(buffer_joueur2 + pos2, sizeof(buffer_joueur2) - pos2, " %2d  ", i);
    }
    pos2 += snprintf(buffer_joueur2 + pos2, sizeof(buffer_joueur2) - pos2, "\n");

    pos2 += snprintf(buffer_joueur2 + pos2, sizeof(buffer_joueur2) - pos2, "  ");
    for (int i = 5; i >= 0; i--)
    {
        pos2 += snprintf(buffer_joueur2 + pos2, sizeof(buffer_joueur2) - pos2, " %2d  ", jeu->plateau[i].nbGraines);
    }
    pos2 += snprintf(buffer_joueur2 + pos2, sizeof(buffer_joueur2) - pos2, "\n");

    pos2 += snprintf(buffer_joueur2 + pos2, sizeof(buffer_joueur2) - pos2, "  ----------------------------\n");

    pos2 += snprintf(buffer_joueur2 + pos2, sizeof(buffer_joueur2) - pos2, "  ");
    for (int i = 6; i < 12; i++)
    {
        pos2 += snprintf(buffer_joueur2 + pos2, sizeof(buffer_joueur2) - pos2, " %2d  ", i);
    }
    pos2 += snprintf(buffer_joueur2 + pos2, sizeof(buffer_joueur2) - pos2, "\n");

    pos2 += snprintf(buffer_joueur2 + pos2, sizeof(buffer_joueur2) - pos2, "  ");
    for (int i = 6; i < 12; i++)
    {
        pos2 += snprintf(buffer_joueur2 + pos2, sizeof(buffer_joueur2) - pos2, " %2d  ", jeu->plateau[i].nbGraines);
    }
    pos2 += snprintf(buffer_joueur2 + pos2, sizeof(buffer_joueur2) - pos2, "\n\n");

    pos2 += snprintf(buffer_joueur2 + pos2, sizeof(buffer_joueur2) - pos2, "%sJoueur 2 :%s %s\n%sScore :%s %d\n\n",
                     COLOR_GREEN, COLOR_RESET,
                     jeu->joueur2->nom,
                     COLOR_GREEN, COLOR_RESET,
                     jeu->joueur2->score);
    // Envoi des messages
    char message1[MAX_MESSAGE_SIZE];
    char message2[MAX_MESSAGE_SIZE];

    snprintf(message1, sizeof(message1), "/message #server #%s %s", jeu->joueur1->nom, buffer_joueur1);
    snprintf(message2, sizeof(message2), "/message #server #%s %s", jeu->joueur2->nom, buffer_joueur2);

    envoyer_message(jeu->joueur1, message1);
    envoyer_message(jeu->joueur2, message2);
}

void envoyer_liste_joueurs(Lobby *lobby, Joueur *joueur)
{
    char *header = "Pseudo\t\t | \tStatus\n";
    int totalLength = strlen(header);

    // Calcule la longueur totale nécessaire pour le message
    for (int i = 0; i < lobby->nbJoueurs; i++)
    {
        totalLength += strlen(lobby->joueurs[i]->nom) + 256; // +20 pour le tab, status et saut de ligne
    }

    char *message = malloc(totalLength * sizeof(char));
    if (!message)
    {
        perror("Erreur d'allocation mémoire");
        return;
    }

    // Construit le message ligne par ligne
    const char *tirets = "|-------------------------------|-------------------------------|\n";
    snprintf(message, totalLength, "|%s\t\tPseudo\t\t%s|%s\t\tStatus%s\t\t|\n", COLOR_BLUE, COLOR_RESET, COLOR_BLUE, COLOR_RESET);
    strcat(message, tirets);
    for (int i = 0; i < lobby->nbJoueurs; i++)
    {
        char ligne[256];
        const char *status;
        if (lobby->joueurs[i]->status == LOBBY)
        {
            status = "Lobby";
        }
        else if (lobby->joueurs[i]->status == INIT)
        {
            status = "Initialisation";
        }
        else
        {
            status = "Partie";
        }
        snprintf(ligne, sizeof(ligne), "|\t\t%s\t\t|\t\t%s\t\t|\n", lobby->joueurs[i]->nom, status);
        strncat(message, ligne, totalLength - strlen(message) - 1);
    }

    char messageToSend[MAX_MESSAGE_SIZE];
    snprintf(messageToSend, sizeof(messageToSend), "/listeJoueurs #server #%s %sListe des joueurs connectés :%s \n%s", joueur->nom, COLOR_YELLOW, COLOR_RESET, message);
    envoyer_message(joueur, messageToSend);

    free(message);
}

void envoyer_defi(Joueur *joueur, Joueur *defie)
{
    char message[MAX_MESSAGE_SIZE];
    snprintf(message, sizeof(message), "/defier #%s #%s Le joueur %s%s%s veut vous défier\n. Pour accepter, %stapez 1%s ; pour refuser, %stapez 0%s.", joueur->nom, defie->nom, COLOR_BLUE, joueur->nom, COLOR_RESET, COLOR_GREEN, COLOR_RESET, COLOR_RED, COLOR_RESET);
    envoyer_message(defie, message);
}

void envoyer_rejoindre(Joueur *joueur, Lobby *lobby)
{
    char message[MAX_MESSAGE_SIZE];
    snprintf(message, sizeof(message), "/rejoindre #server #%s Un nouveau joueur a rejoint le lobby : %s%s%s\n", joueur->nom, COLOR_GREEN, joueur->nom, COLOR_RESET);
    envoyer_a_tous(lobby, message);
}

void envoyer_message_global(Joueur *joueur, Lobby *lobby, char body[MAX_BODY_SIZE])
{
    for (int i = 0; i < lobby->nbJoueurs; i++)
    {
        Joueur *destinataire = lobby->joueurs[i];
        if (destinataire != NULL && destinataire->status == LOBBY && destinataire != joueur)
        {
            char message[MAX_MESSAGE_SIZE];
            snprintf(message, sizeof(message),
                     "/messageGlobal #server #%s Message de %s%s%s : %s%s%s\n",
                     joueur->nom,
                     COLOR_GREEN, joueur->nom, COLOR_RESET,
                     COLOR_BLUE, body, COLOR_RESET);

            envoyer_message(destinataire, message);
        }
    }
}

void envoyer_decliner_defi(Joueur *joueur, Joueur *demandeur)
{
    char message[MAX_MESSAGE_SIZE];
    snprintf(message, sizeof(message), "/message #server #%s %s a %srefusé%s votre défi.", demandeur->nom, joueur->nom, COLOR_RED, COLOR_RESET);
    envoyer_message(demandeur, message);
}
void envoyer_accepter_defi(Joueur *joueur, Joueur *demandeur)
{
    char message[MAX_MESSAGE_SIZE];
    snprintf(message, sizeof(message), "/message #server #%s %s a %saccepté%s votre défi.", demandeur->nom, joueur->nom, COLOR_GREEN, COLOR_RESET);
    envoyer_message(demandeur, message);
}

void envoyer_bio(Joueur *joueur)
{
    char message[MAX_MESSAGE_SIZE];
    snprintf(message, sizeof(message), "/message #server #%s Votre bio a été %smodifiée%s : %s", joueur->nom, COLOR_BLUE, COLOR_RESET, joueur->bio);
    envoyer_message(joueur, message);
}

void envoyer_consulter_bio(Joueur *demandeur, Joueur *jbio)
{
    char message[MAX_MESSAGE_SIZE];
    snprintf(message, sizeof(message), "/message #server #%s Voici la bio de %s%s%s :\n %s%s%s", demandeur->nom, COLOR_BLUE, jbio->nom, COLOR_RESET, COLOR_GREEN, jbio->bio, COLOR_RESET);
    envoyer_message(demandeur, message);
}
void envoyer_elo_joueurs(Jeu *jeu)
{
    char message[MAX_MESSAGE_SIZE];
    snprintf(message, sizeof(message), "/message #server #%s Votre ELO est de %s%f%s", jeu->joueur1->nom, COLOR_BLUE, jeu->joueur1->elo, COLOR_RESET);
    envoyer_message(jeu->joueur1, message);
    snprintf(message, sizeof(message), "/message #server #%s Votre ELO est de %s%f%s", jeu->joueur2->nom, COLOR_BLUE, jeu->joueur2->elo, COLOR_RESET);
    envoyer_message(jeu->joueur2, message);
}

void envoyer_classement(Lobby *lobby, Joueur *joueur)
{
    char buffer[MAX_BODY_SIZE];
    int pos = 0;

    // Ajout du header
    pos += snprintf(buffer + pos, MAX_BODY_SIZE - pos, "%sPseudo%s\t\t\t | \t\t%sELO%s\n", COLOR_BLUE, COLOR_RESET, COLOR_BLUE, COLOR_RESET);
    pos += snprintf(buffer + pos, MAX_BODY_SIZE - pos, "-------------------------|-------------------------\n");

    // Ajout des joueurs et de leur ELO
    for (int i = 0; i < lobby->nbJoueurs && pos < MAX_BODY_SIZE; i++)
    {
        pos += snprintf(buffer + pos, MAX_BODY_SIZE - pos, "%s\t\t\t | \t\t%.2f\n",
                        lobby->joueurs[i]->nom, lobby->joueurs[i]->elo);
    }

    char messageToSend[MAX_MESSAGE_SIZE];
    snprintf(messageToSend, sizeof(messageToSend), "/message #server #%s %sClassement des joueurs :%s \n%s", joueur->nom, COLOR_YELLOW, COLOR_RESET, buffer);

    // Envoi du message
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
