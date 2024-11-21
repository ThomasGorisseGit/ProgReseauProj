
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
    snprintf(message, sizeof(message), "/message #server #%s %sErreur :%s %sLe joueur n'existe pas ou n'est pas connecté.%s", joueur->nom, COLOR_RED_BOLD, COLOR_RESET, COLOR_RED, COLOR_RESET);
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

    char buffer[MAX_BODY_SIZE];
    int pos = 0;
    pos += snprintf(buffer + pos, sizeof(buffer) - pos, "%sJoueur 2 :%s %s\n%sScore :%s %d\n\n",
                    COLOR_GREEN, COLOR_RESET,
                    jeu->joueur2->nom,
                    COLOR_GREEN, COLOR_RESET,
                    jeu->joueur2->score);
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
    pos += snprintf(buffer + pos, sizeof(buffer) - pos, "%sJoueur 1 :%s %s\n%sScore :%s %d\n",
                    COLOR_GREEN, COLOR_RESET,
                    jeu->joueur1->nom,
                    COLOR_GREEN, COLOR_RESET,
                    jeu->joueur1->score);

    char message[MAX_MESSAGE_SIZE];
    snprintf(message, sizeof(message), "/message #server #%s %s", jeu->joueur1->nom, buffer);
    envoyer_message(jeu->joueur1, message);
    envoyer_message(jeu->joueur2, message);
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
            status = "En partie";
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