#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>

#include "../model/donnees/constants.h"
#include "../model/services/lobbyManager.h"
#include "../model/services/messageManager.h"
#include "../vue/echange.h"
#include "../model/donnees/awale.h"
#include "../model/services/partieManager.h"
#include "commandes.h"

Lobby *lobby;

void message_handler(Joueur *joueur, char *message)
{
    char command[MAX_COMMAND_SIZE], destinataire[MAX_DESTINATAIRE_SIZE], body[MAX_BODY_SIZE], expediteur[MAX_DESTINATAIRE_SIZE];
    int valid_message = verifierFormatMessage(message, command, expediteur, destinataire, body);
    if (!valid_message)
    {
        envoyer_erreur(joueur, "Message invalide");
        return;
    }
    if (strcmp(command, "nom") == 0)
    {
        commande_nom(joueur, lobby, body);
    }

    else if (strcmp(command, "listeJoueurs") == 0)
    {
        commande_listeJoueurs(joueur, lobby);
    }

    else if (strcmp(command, "defier") == 0)
    {
        commande_defier(lobby, joueur, destinataire);
    }

    else if (strcmp(command, "declinerDefi") == 0)
    {
        commande_declinerDefi(lobby, joueur, destinataire);
    }

    else if (strcmp(command, "accepterDefi") == 0)
    {
        commande_accepterDefi(lobby, joueur, destinataire);
    }

    else if (strcmp(command, "message") == 0)
    {
        commande_message(destinataire, expediteur, lobby, body);
    }
    else if (strcmp(command, "coup") == 0)
    {
        // int case_depart = atoi(body);
        // Jeu *jeu = lobby->jeux[joueur->idPartie];
        commande_jouerCoup(lobby, joueur, body);
    }
    else if (strcmp(command, "modifierBio") == 0)
    {
        commande_modifierBio(joueur, body);
    }
    else if (strcmp(command, "consulterBio") == 0)
    {
        commande_consulterBio(joueur, lobby, body);
    }
    else if (strcmp(command, "classement") == 0)
    {
        commande_classement(joueur, lobby);
    }
    else if (strcmp(command, "messageGlobal") == 0)
    {
        commande_message_global(lobby, expediteur, body);
    }
    else
    {
        printf("Commande inconnue reçue: %s\n", command);
    }
}

int check_if_message(Lobby *lobby, fd_set *readfds, fd_set *masterfds)
{
    for (int i = 0; i < lobby->nbJoueurs; i++)
    {
        Joueur *joueur = lobby->joueurs[i];
        char message[MAX_MESSAGE_SIZE];
        int n;

        if (FD_ISSET(*joueur->socket, readfds))
        {
            n = read(*joueur->socket, message, MAX_MESSAGE_SIZE);
            if (n < 0)
            {
                perror("Erreur de lecture du socket");
                continue;
            }
            if (n == 0)
            {
                printf("Client déconnecté\n");
                close(*joueur->socket);
                FD_CLR(*joueur->socket, masterfds);
                free(joueur->nom);
                free(joueur);
                lobby->joueurs[i] = lobby->joueurs[--lobby->nbJoueurs];
                i--;
                continue;
            }

            message[n] = '\0';
            printf("\n\nMessage reçu de %s: %s\n\n", joueur->nom, message);
            message_handler(joueur, message);
        }
    }

    return 0;
}
int main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in serv_addr;
    fd_set readfds, masterfds;
    int max_fd;

    if (argc != 2)
    {
        printf("usage: socket_server port\n");
        exit(0);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("Impossible d'ouvrir le socket");
        exit(0);
    }

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Impossible de faire le bind");
        exit(0);
    }

    listen(sockfd, MAX_PLAYER);
    printf("En attente de connexions sur le port %d\n", atoi(argv[1]));

    lobby = initialiserLobby();

    // Initialisation des ensembles
    FD_ZERO(&masterfds);
    FD_ZERO(&readfds);
    FD_SET(sockfd, &masterfds);
    max_fd = sockfd;

    while (1)
    {
        // Copier masterfds dans readfds avant select
        readfds = masterfds;

        int activity = select(max_fd + 1, &readfds, NULL, NULL, NULL);

        if (activity < 0 && errno != EINTR)
        {
            perror("Erreur avec select");
            break;
        }

        // Gérer les nouvelles connexions
        if (FD_ISSET(sockfd, &readfds))
        {
            handle_connection(sockfd, &masterfds, &max_fd, lobby);
        }

        // Gérer les messages des clients
        check_if_message(lobby, &readfds, &masterfds);

        // Mettre à jour masterfds et max_fd dans handle_connection et check_if_message
    }

    close(sockfd);
    return 0;
}
