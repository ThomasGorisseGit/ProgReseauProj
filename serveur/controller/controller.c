#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>
#include <stdlib.h>

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
    else
    {
        printf("Commande inconnue reçue: %s\n", command);
    }
}

int check_if_message(Lobby *lobby, fd_set *readfds)
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
                FD_CLR(*joueur->socket, readfds);
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
    fd_set readfds;
    int max_fd;
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

    listen(sockfd, MAX_PLAYER);
    printf("En attente de connexions\n");

    lobby = initialiserLobby();
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
        check_if_message(lobby, &copyfds);
    }
    close(sockfd);
    return 1;
}