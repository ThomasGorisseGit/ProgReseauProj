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

// Définition des couleurs pour terminal
#define COLOR_RESET "\033[0m"
#define COLOR_RED "\033[31m"
#define COLOR_GREEN "\033[32m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_BLUE "\033[34m"

// Affiche le guide d'utilisation des commandes
void afficher_guide()
{
    printf(COLOR_YELLOW "Commandes disponibles:\n" COLOR_RESET);
    printf(COLOR_GREEN "/listeJoueurs" COLOR_RESET " - Liste les joueurs dans le lobby\n");
    printf(COLOR_GREEN "/defier <nom>" COLOR_RESET " - Défi un joueur spécifié par son nom\n");
}
void handle_message(char *message, int *sockfd)
{
    char command[50], destinataire[50], body[150], expediteur[50];
    if (verifierFormatMessage(message, command, destinataire, body, expediteur))
    {
        if (strcmp(command, "joining") == 0)
        {
            printf(COLOR_GREEN "%s\n" COLOR_RESET, body);
        }
        else if (strcmp(command, "displayLobby") == 0)
        {
            printf(COLOR_BLUE "%s\n" COLOR_RESET, body);
        }
        else if (strcmp(command, "defier") == 0)
        {
            printf(COLOR_RED "Défi reçu de %s : %s\n" COLOR_RESET, expediteur, body);
            // Demander une réponse de l'utilisateur
            char response[10];
            fgets(response, sizeof(response), stdin);
            response[strcspn(response, "\n")] = 0;

            if (strcmp(response, "1") == 0)
            {
                ecrire(sockfd, "accepterDefi", expediteur, "Le défi est accepté.", destinataire);
            }
            else
            {
                ecrire(sockfd, "declinerDefi", expediteur, "Le défi est refusé.", destinataire);
            }
        }
        else if (strcmp(command, "listeJoueurs") == 0)
        {
            printf(COLOR_BLUE "La liste des joueurs disponibles est la suivante : %s\n" COLOR_RESET, body);
        }
        else if (strcmp(command, "message") == 0)
        {
            printf(COLOR_YELLOW "%s\n" COLOR_RESET, body);
        }
        else
        {
            printf(COLOR_RED "Commande inconnue du serveur\n" COLOR_RESET);
            printf("%s\n", message);
        }
    }
    else
    {
        printf(COLOR_RED "Message du serveur invalide : %s\n" COLOR_RESET, message);
        printf("%s\n", message);
    }
}

int main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in serv_addr;

    if (argc != 3)
    {
        printf(COLOR_RED "usage: socket_client server port\n" COLOR_RESET);
        exit(0);
    }

    printf(COLOR_BLUE "Client starting...\n" COLOR_RESET);

    // Initialisation du serveur
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

    // Demander le nom de l'utilisateur avant de rejoindre le lobby
    char nom[50];
    printf(COLOR_BLUE "Entrez votre nom : " COLOR_RESET);
    fgets(nom, sizeof(nom), stdin);
    nom[strcspn(nom, "\n")] = '\0'; // Supprimer le saut de ligne

    // Envoyer la commande de nom au serveur avec l’expéditeur
    ecrire(&sockfd, "name", "server", nom, nom);

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

        // Lecture des messages du serveur
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
                printf(COLOR_RED "Serveur déconnecté.\n" COLOR_RESET);
                break;
            }
            else
            {
                perror("Erreur de lecture depuis le serveur");
            }
        }

        // Lecture des commandes utilisateur
        if (FD_ISSET(STDIN_FILENO, &readfds))
        {
            char user_input[256];
            fgets(user_input, sizeof(user_input), stdin);
            user_input[strcspn(user_input, "\n")] = 0; // Supprime le saut de ligne

            // Vérifie la commande et l'envoie au serveur avec le nom comme expéditeur
            if (strcmp(user_input, "/listeJoueurs") == 0)
            {
                ecrire(&sockfd, "listeJoueurs", "server", "", nom);
            }
            else if (strncmp(user_input, "/defier ", 7) == 0)
            {
                char *target = user_input + 8; // Extrait le nom cible
                ecrire(&sockfd, "defier", target, "", nom);
            }
            else if (strcmp(user_input, "/help") == 0)
            {
                afficher_guide();
            }
            else
            {
                printf(COLOR_YELLOW "Commande inconnue. Utilisez /help pour voir les commandes disponibles.\n" COLOR_RESET);
            }
        }
    }

    close(sockfd);
    return 0;
}
