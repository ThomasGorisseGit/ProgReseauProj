#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "communication.h"
#include "util.h"

void handle_message(char *message, int *sockfd)
{
    char command[50], destinataire[50], body[150];
    if (verifierFormatMessage(message, command, destinataire, body))
    {
        if (strcmp(command, "name") == 0)
        {
            printf("%s\n", body);
            ecrire(sockfd, command, destinataire);  // `ecrire` demandera `body` dans `stdin`
        }
        else if (strcmp(command, "joining") == 0)
        {
            printf("%s\n", body);
        }
        else if (strcmp(command, "displayLobby") == 0)
        {
            printf("%s\n", body);
        }
        else if (strcmp(command, "defi") == 0)
        {
            printf("Défi reçu de %s\n", destinataire);
            ecrire(sockfd, command, destinataire);
        }
        else if (strcmp(command, "listeJoueurs") == 0)
        {
            printf("Liste des joueurs disponibles :\n%s\n", body);
        }
        else
        {
            printf("Commande inconnue du serveur\n");
        }
    }
    else
    {
        printf("Message du serveur invalide : %s\n", message);
    }
}

int main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in serv_addr;

    if (argc != 3)
    {
        printf("usage: socket_client server port\n");
        exit(0);
    }

    printf("Client starting...\n");

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket error");
        exit(0);
    }

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Connect error");
        exit(0);
    }


    fd_set readfds;
    char buffer[1024];

    while (1)
    {
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);
        FD_SET(STDIN_FILENO, &readfds);

        int max_fd = sockfd > STDIN_FILENO ? sockfd : STDIN_FILENO;
        int activity = select(max_fd + 1, &readfds, NULL, NULL, NULL);

        if (activity < 0)
        {
            perror("select error");
            break;
        }

        if (FD_ISSET(sockfd, &readfds))
        {
            int n = read(sockfd, buffer, sizeof(buffer) - 1);
            if (n > 0)
            {
                buffer[n] = '\0';
                handle_message(buffer, &sockfd);
            }
            else if (n == 0)
            {
                printf("Serveur déconnecté.\n");
                break;
            }
            else
            {
                perror("Erreur de lecture depuis le serveur");
            }
        }

        if (FD_ISSET(STDIN_FILENO, &readfds))
        {
            char user_input[256];
            fgets(user_input, sizeof(user_input), stdin);
            user_input[strcspn(user_input, "\n")] = 0;

            if (strcmp(user_input, "/listeJoueurs") == 0)
            {
                ecrire(&sockfd, "listeJoueurs", "server");
            }
            else if (strncmp(user_input, "/defi ", 6) == 0)
            {
                char *target = user_input + 6;
                ecrire(&sockfd, "defi", target);
            }
            else
            {
                printf("Commande inconnue. Utilisez /listeJoueurs ou /defi <nom>\n");
            }
        }
    }

    close(sockfd);
    return 0;
}
