#include "lobbyManager.h"
#include <stdio.h>
#include <stdlib.h>
Lobby *initialiserLobby()
{

    Lobby *lobby = malloc(sizeof(Lobby));

    lobby->joueurs = malloc(MAX_PLAYER * sizeof(Joueur *));
    lobby->nbJoueurs = 0;
    lobby->jeux = malloc(MAX_PARTIES * sizeof(Jeu *));
    lobby->nbJeux = 0;

    return lobby;
}

void handle_connection(int sockfd, fd_set *readfds, int *max_fd, Lobby *lobby)
{
    struct sockaddr_in cli_addr;
    socklen_t clilen = sizeof(cli_addr);
    int *newsockfd = malloc(sizeof(int));

    *newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    if (*newsockfd < 0)
    {
        perror("ERROR on accept");
        free(newsockfd);
        return;
    }

    printf("Nouvelle connexion acceptée\n");

    Joueur *joueur = malloc(sizeof(Joueur));
    joueur->socket = newsockfd;
    joueur->nom = malloc(MAX_NAME_SIZE * sizeof(char));

    strcpy(joueur->nom, "Anonyme");
    joueur->status = INIT;
    joueur->idPartie = -1;
    joueur->idJoueur = lobby->nbJoueurs;

    // Ajouter le nouveau joueur au lobby
    lobby->joueurs[lobby->nbJoueurs] = joueur;
    lobby->nbJoueurs++;

    // Ajouter le socket du joueur à l'ensemble des descripteurs à surveiller
    FD_SET(*newsockfd, readfds);
    if (*newsockfd > *max_fd)
        *max_fd = *newsockfd;
}

void fin_partie(Jeu *jeu)
{
    jeu->joueur1->status = LOBBY;
    jeu->joueur2->status = LOBBY;

    jeu->joueur1->idPartie = -1;
    jeu->joueur2->idPartie = -1;

    free(jeu);
    jeu = NULL;
}

Joueur *trouver_joueur(Lobby *lobby, const char *nom)
{
    for (int i = 0; i < lobby->nbJoueurs; i++)
    {
        if (strcmp(lobby->joueurs[i]->nom, nom) == 0) // Comparaison des noms
        {
            return lobby->joueurs[i]; // Retourne le joueur trouvé
        }
    }
    return NULL; // Retourne NULL si le joueur n'est pas trouvé
}

Joueur *defier_joueur(Lobby *lobby, char *pseudo)
{
    for (int i = 0; i < lobby->nbJoueurs; i++)
    {
        Joueur *joueur = lobby->joueurs[i];

        // Vérifie si le pseudo correspond
        if (strcmp(joueur->nom, pseudo) == 0)
        {
            // Vérifie si le joueur est dans le lobby et disponible
            if (joueur->status == LOBBY)
            {
                return joueur; // Retourne le pointeur vers le joueur disponible
            }
            else
            {
                return NULL; // Le joueur est en partie
            }
        }
    }
    return NULL;
}

int commande_listeJoueurs(Joueur *joueur, Lobby *lobby)
{
    char liste_message[MAX_MESSAGE_SIZE];
    snprintf(liste_message, sizeof(liste_message), "/listeJoueurs #server #%s %s", joueur->nom, "TODO :Liste des joueurs");
    return envoyer_message(joueur, liste_message);
}

int commande_nom(Joueur *joueur, Lobby *lobby, char body[MAX_BODY_SIZE])
{
    strcpy(joueur->nom, body);
    joueur->status = LOBBY;

    char join_msg[MAX_MESSAGE_SIZE];
    snprintf(join_msg, sizeof(join_msg), "/joining #server #%s Un nouveau joueur a rejoint le lobby : %s\n", joueur->nom, joueur->nom);

    printf("Nouveau joueur : %s\n", joueur->nom);

    envoyer_a_tous(lobby, join_msg);
    usleep(2000);

    return commande_listeJoueurs(joueur, lobby);
}

int commande_message(char destinataire[MAX_DESTINATAIRE_SIZE], char expediteur[MAX_DESTINATAIRE_SIZE], Lobby *lobby, char body[MAX_BODY_SIZE])
{
    Joueur *destinataireJoueur = trouver_joueur(lobby, destinataire);
    int result;
    if (destinataireJoueur != NULL)
    {
        // Format the message to be sent to the target player
        char msg[MAX_MESSAGE_SIZE];
        snprintf(msg, sizeof(msg), "/message #%s #%s %s", expediteur, destinataire, body);
        result = envoyer_message(destinataireJoueur, msg);
        printf("Message envoyé à %s de la part de %s: %s\n", destinataire, expediteur, body);
    }
    else
    {
        // Send an error message back to the sender if the recipient does not exist
        char error_msg[MAX_MESSAGE_SIZE];
        snprintf(error_msg, sizeof(error_msg), "/message #server #%s Le joueur %s n'existe pas ou n'est pas connecté.", expediteur, destinataire);
        Joueur *j_expediteur = trouver_joueur(lobby, expediteur);
        result = envoyer_message(j_expediteur, error_msg);
        printf("Erreur : Le joueur %s n'existe pas ou n'est pas connecté.\n", destinataire);
    }
    return result;
}

int commande_defier(Lobby *lobby, Joueur *joueur, char destinataire[MAX_DESTINATAIRE_SIZE])
{
    Joueur *defie = defier_joueur(lobby, destinataire);
    int result;
    if (strcmp(joueur->nom, destinataire) == 0)
    {
        envoyer_erreur(joueur);
        // char defier_msg[MAX_MESSAGE_SIZE];
        // snprintf(defier_msg, sizeof(defier_msg), "/message #%s #%s Vous ne pouvez pas vous défier vous-même.", joueur->nom, joueur->nom);
        // result = envoyer_message(joueur, defier_msg);
    }
    else if (defie != NULL)
    {
        defie->status = DEFI;
        joueur->status = DEFI;
        envoyer_defi(joueur, defie);
        // char defier_msg[MAX_MESSAGE_SIZE];
        // snprintf(defier_msg, sizeof(defier_msg), "/defier #%s #%s Le joueur %s veut vous défier. Pour accepter, tapez 1 ; pour refuser, tapez 0.", joueur->nom, defie->nom, joueur->nom);
        // result = envoyer_message(defie, defier_msg);
    }
    else
    {
        envoyer_erreur(joueur);
        // char error_msg[MAX_MESSAGE_SIZE];
        // snprintf(error_msg, sizeof(error_msg), "/message #serveur #%s Le joueur %s n'est pas disponible pour un défi.", joueur->nom, destinataire);
        // result = envoyer_message(joueur, error_msg);
    }
    return result;
}

int commande_declinerDefi(Lobby *lobby, Joueur *joueur, char destinataire[MAX_DESTINATAIRE_SIZE])
{
    Joueur *demandeur = trouver_joueur(lobby, destinataire);
    int result;
    if (demandeur != NULL)
    {
        envoyer_decliner_defi(joueur, demandeur);
        demandeur->status = LOBBY;
        joueur->status = LOBBY;
    }
    return result;
}