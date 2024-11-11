#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>
#include <assert.h>
#include "awale.h"
#include "util.h"
#include "communication.h"
#include "server.h"
#include "partie.h"

char *toStringLobby(Lobby *lobby)
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
    return message;
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
    joueur->status = malloc(sizeof(Status));

    strcpy(joueur->nom, "Anonyme");
    joueur->status = LOBBY;

    // Ajouter le nouveau joueur au lobby
    lobby->joueurs[lobby->nbJoueurs] = joueur;
    lobby->nbJoueurs++;

    // Ajouter le socket du joueur à l'ensemble des descripteurs à surveiller
    FD_SET(*newsockfd, readfds);
    if (*newsockfd > *max_fd)
        *max_fd = *newsockfd;

    envoyer(joueur, "/name #server Entrez votre nom : ");
}
void handle_message(Lobby *lobby, fd_set *readfds)
{
    for (int i = 0; i < lobby->nbJoueurs; i++)
    {
        Joueur *joueur = lobby->joueurs[i];
        char message[MAX_MESSAGE_SIZE];
        int n;

        // Vérifie si le joueur a envoyé un message
        if (FD_ISSET(*joueur->socket, readfds))
        {
            n = read(*joueur->socket, message, MAX_MESSAGE_SIZE);
            if (n < 0)
            {
                perror("Erreur de lecture du socket");
                continue;
            }
            if (n == 0) // Si le client se déconnecte
            {
                printf("Client déconnecté\n");
                if (*joueur->socket >= 0)
                {
                    FD_CLR(*joueur->socket, readfds);
                    close(*joueur->socket);
                }
                free(joueur->nom);
                free(joueur);
                lobby->joueurs[i] = lobby->joueurs[lobby->nbJoueurs - 1];
                lobby->nbJoueurs--;
                i--;
                continue;
            }

            // Assure que le message est une chaîne de caractères valide
            message[n] = '\0';

            // Affiche le message brut reçu dans la console du serveur
            printf("Message reçu de %s: %s\n", joueur->nom, message);

            // Analyse le message pour extraire commande, destinataire et corps
            char command[MAX_COMMAND_SIZE], destinataire[MAX_DESTINATAIRE_SIZE], body[MAX_BODY_SIZE];
            if (verifierFormatMessage(message, command, destinataire, body))
            {
                // Traitement des différentes commandes
                if (strcmp(command, "name") == 0)
                {
                    strcpy(joueur->nom, body);
                    printf("Nom du joueur %d mis à jour : %s\n", i, joueur->nom);

                    char join_msg[MAX_MESSAGE_SIZE];
                    snprintf(join_msg, sizeof(join_msg), "/joining #server Un nouveau joueur a rejoint le lobby : %s", joueur->nom);
                    envoyerATousDansLobby(lobby, join_msg);

                    char lobby_msg[MAX_MESSAGE_SIZE];
                    snprintf(lobby_msg, sizeof(lobby_msg), "/displayLobby #server %s", toStringLobby(lobby));
                    envoyer(joueur, lobby_msg);
                }
                else if (strcmp(command, "listeJoueurs") == 0)
                {
                    // Affiche la liste des joueurs dans le lobby
                    char liste_message[MAX_MESSAGE_SIZE];
                    snprintf(liste_message, sizeof(liste_message), "/listeJoueurs #server %s", toStringJoueursDispo(lobby));
                    envoyer(joueur, liste_message);
                }
                else if (strcmp(command, "defier") == 0)
                {
                    printf("Le corps du message est : %s\n", body);
                    Joueur *defie = defierJoueur(lobby, body);
                    if (strcmp(joueur->nom, body) == 0)
                    {
                        char defier_msg[MAX_MESSAGE_SIZE];
                        snprintf(defier_msg, sizeof(defier_msg), "/defier #%s Vous ne pouvez pas vous défier vous même.", joueur->nom);
                        envoyer(joueur, defier_msg);
                    }
                    else if (defie != NULL)
                    {
                        char defier_msg[MAX_MESSAGE_SIZE];
                        snprintf(defier_msg, sizeof(defier_msg), "/defier #%s Le joueur %s veut vous défier. Pour accepter tapez 1, pour refuser tapez 0.", defie->nom, joueur->nom);
                        envoyer(defie, defier_msg);
                    }
                    else
                    {
                        char error_msg[MAX_MESSAGE_SIZE];
                        snprintf(error_msg, sizeof(error_msg), "/error #server Le joueur %s n'est pas disponible pour un défi.", body);
                        envoyer(joueur, error_msg);
                    }
                }

                else
                {
                    printf("Commande inconnue reçue: %s\n", command);
                }
            }
            else
            {
                printf("Format de message invalide reçu: %s\n", message);
            }
        }
    }
}

int main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;

    fd_set readfds;
    int max_fd;
    int i, n;
    Joueur *joueurs[MAX_PLAYER];
    int nb_joueurs = 0;
    if (argc != 2)
    {
        printf("usage: socket_server port\n");
        exit(0);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("impossible d'ouvrir le socket\n");
        exit(0);
    }

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));
    printf("port : %d\n", atoi(argv[1]));

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("impossible de faire le bind\n");
        exit(0);
    }

    listen(sockfd, 5);
    printf("En attente de connexions\n");

    Lobby *lobby = malloc(sizeof(Lobby));
    lobby->joueurs = malloc(MAX_PLAYER * sizeof(Joueur *));
    lobby->nbJoueurs = 0;

    FD_ZERO(&readfds);
    FD_SET(sockfd, &readfds);
    max_fd = sockfd;
    while (1)
    {
        fd_set copyfds = readfds;
        int activity = select(max_fd + 1, &copyfds, NULL, NULL, NULL);

        if (activity < 0)
        {
            perror("select error");
            exit(1);
        }
        if (FD_ISSET(sockfd, &copyfds))
        {
            handle_connection(sockfd, &readfds, &max_fd, lobby);
        }

        handle_message(lobby, &copyfds);
    }
    close(sockfd);
    return 1;
}