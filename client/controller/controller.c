#include "controller.h"

int nomSet = 0;

void sigint_handler(int sig) {
    exit(0);
}
void handle_server_message(char *message, int *sockfd)
{
    char command[MAX_COMMAND_SIZE], destinataire[MAX_DESTINATAIRE_SIZE], body[MAX_BODY_SIZE], expediteur[MAX_DESTINATAIRE_SIZE];

    if (verifierFormatMessage(message, command, expediteur, destinataire, body))
    {
        if (strcmp(command, "rejoindre") == 0)
        {
            afficher_message_nocolour(body);
        }
        else if (strcmp(command, "finPartie") == 0)
        {
            char formatted_message[MAX_MESSAGE_SIZE];
            snprintf(formatted_message, sizeof(formatted_message), "La partie est terminée : %s", body);
            afficher_message(COLOR_GREEN, formatted_message);
        }

        else if (strcmp(command, "defier") == 0)
        {
            char formatted_message[MAX_MESSAGE_SIZE];
            snprintf(formatted_message, sizeof(formatted_message), "Défi reçu de %s : %s", expediteur, body);
            // afficher_message(COLOR_RED, formatted_message);
            afficher_message_nocolour(formatted_message);

            // Demande de réponse utilisateur
            char response[10];
            // afficher_message(COLOR_RESET, "Accepter le défi ? (1 pour oui, 0 pour non) : ");
            fgets(response, sizeof(response), stdin);
            response[strcspn(response, "\n")] = '\0';

            // Envoi de la réponse
            snprintf(formatted_message, sizeof(formatted_message),
                     strcmp(response, "1") == 0 ? "Le défi est accepté." : "Le défi est refusé.");
            ecrire(sockfd, strcmp(response, "1") == 0 ? "accepterDefi" : "declinerDefi", destinataire, expediteur, formatted_message);
        }
        else if (strcmp(command, "listeJoueurs") == 0)
        {
            // afficher_message(COLOR_BLUE, body);
            afficher_message_nocolour(body);
        }
        else if (strcmp(command, "message") == 0)
        {
            char formatted_message[MAX_MESSAGE_SIZE];
            snprintf(formatted_message, sizeof(formatted_message), "Message de %s : \n%s", expediteur, body);
            // afficher_message(COLOR_RED, formatted_message);
            afficher_message_nocolour(formatted_message);
        }
        else if (strcmp(command, "messageGlobal") == 0)
        {
            afficher_message_nocolour(body);
        }
        else if (strcmp(command, "case") == 0)
        {
            afficher_message(COLOR_GREEN, body);
            char *input = lireInput();
            //if input starts with /
            /*
            if (input[0] == '/')
            {
                handle_client_input(body, sockfd, "server");
                
            }else{
                ecrire(sockfd, "coup", destinataire, expediteur, input);
            }
            */
            ecrire(sockfd, "coup", destinataire, expediteur, input);
        }
        else if (strcmp(command, "nomValide") == 0)
        {
            nomSet = 1;
            afficher_message(COLOR_GREEN, body);
            afficher_guide();
        }
        else if (strcmp(command, "nomInvalide") == 0)
        {
            afficher_message(COLOR_RED, body);
        }
        else if (strcmp(command, "classement") == 0)
        {
            afficher_message(COLOR_BLUE, body);
        }
        else
        {
            afficher_message(COLOR_YELLOW, "Commande inconnue du serveur.");
        }
    }
    else
    {
        afficher_message(COLOR_RED, "Message du serveur invalide.");
    }
}
// Gère les entrées utilisateur
void handle_client_input(char *user_input, int *sockfd, char *nom)
{

    if (strcmp(user_input, "/listeJoueurs") == 0)
    {
        ecrire(sockfd, "listeJoueurs", nom, "server", "");
    }
    else if (strncmp(user_input, "/defier ", 8) == 0)
    {
        char *target = user_input + 8;
        ecrire(sockfd, "defier", nom, target, "");
    }
    else if (strcmp(user_input, "/defierBot") == 0)
    {
        ecrire(sockfd, "defierBot", nom, "server", "");
    }
    else if (strcmp(user_input, "/aide") == 0)
    {
        afficher_guide();
    }
    else if (strncmp(user_input, "/modifierBio ", 13) == 0)
    {
        char *bio = user_input + 13;
        ecrire(sockfd, "modifierBio", nom, "server", bio);
    }
    else if (strncmp(user_input, "/consulterBio ", 14) == 0)
    {
        char *user = user_input + 14;
        ecrire(sockfd, "consulterBio", nom, "server", user);
    }
    else if (strcmp(user_input, "/classement") == 0)
    {
        ecrire(sockfd, "classement", nom, "server", "");
    }
    else if(strcmp(user_input, "/forfait") == 0){
        ecrire(sockfd, "forfait", nom, "server", "l'adversaire s'est retiré de la partie.");
        return;
    }
    else if (strncmp(user_input, "/messageGlobal", 14) == 0)
        if (strlen(user_input) > 15)
        {
            char *input = user_input + 15;
            while (isspace((unsigned char)*input))
                input++;

            if (strlen(input) > 0)
            {
                ecrire(sockfd, "messageGlobal", nom, "server", input);
            }
            else
            {
                afficher_message(COLOR_YELLOW, "Erreur : Le message global est vide. Utilisez : /messageGlobal <message>\n");
            }
        }
        else
        {
            afficher_message(COLOR_YELLOW, "Erreur : Le message global est vide. Utilisez : /messageGlobal <message>\n");
        }

    else if (strncmp(user_input, "/message ", 9) == 0)
    {
        char *input = user_input + 9;
        char *colon_pos = strchr(input, ':');
        if (colon_pos != NULL)
        {
            *colon_pos = '\0';
            char *target = input;          // Récupère le destinataire
            char *message = colon_pos + 1; // Récupère le contenu du message
            while (isspace((unsigned char)*message))
                message++; // Supprime les espaces avant le contenu
            if (strlen(target) > 0 && strlen(message) > 0)
            {
                ecrire(sockfd, "message", nom, target, message); // Corrige l'ordre des paramètres
            }
            else
            {
                afficher_message(COLOR_YELLOW, "Erreur : /message nécessite un destinataire et un contenu.");
            }
        }
        else
        {
            afficher_message(COLOR_YELLOW, "Erreur : Utilisez un ':' pour séparer le destinataire et le message.");
        }
    }
    else if (strcmp(user_input, "/forfait") == 0)
    {
        ecrire(sockfd, "forfait", nom, "server", "Je me retire de la partie.");
    }

    else
    {
        afficher_message(COLOR_YELLOW, "Commande inconnue. Tapez /aide pour voir les commandes disponibles.");
    }
}
// Initialise la connexion au serveur
int init_connection(const char *server_ip, const char *server_port)
{
    int sockfd;
    struct sockaddr_in serv_addr;

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(server_ip);
    serv_addr.sin_port = htons(atoi(server_port));

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Erreur de création de socket");
        exit(EXIT_FAILURE);
    }

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Erreur de connexion au serveur");
        exit(EXIT_FAILURE);
    }

    return sockfd;
}

// Gère la communication avec le serveur et l'utilisateur
void event_loop(int sockfd, char *nom)
{
    fd_set readfds;

    while (1)
    {
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);
        FD_SET(STDIN_FILENO, &readfds);

        if (select(sockfd + 1, &readfds, NULL, NULL, NULL) < 0)
        {
            perror("Erreur avec select");
            break;
        }

        // Gestion des messages du serveur
        if (FD_ISSET(sockfd, &readfds))
        {
            char buffer[1024];
            int n = read(sockfd, buffer, sizeof(buffer) - 1);
            if (n > 0)
            {
                buffer[n] = '\0';
                handle_server_message(buffer, &sockfd);
            }
            else if (n==0)
            {
                afficher_message(COLOR_RED, "Le serveur s'est déconnecté.");
                break;
            }
            else
            {
                // Erreur de lecture
                perror("Erreur de lecture sur la socket");
                break;  // Quitter la boucle en cas d'erreur
            }
        }

        // Gestion des entrées utilisateur
        if (FD_ISSET(STDIN_FILENO, &readfds))
        {

            char *user_input = lireInput();
            handle_client_input(user_input, &sockfd, nom);
        }
    }
}

char *register_user(int sockfd)
{

    // afficher_message(COLOR_BLUE, "Entrez votre nom :");
    afficher_message_nocolour("Entrez votre nom :");
    char *nom = lireInput();
    ecrire(&sockfd, "nom", nom, "server", nom);

    // Attente d'une réponse
    char buffer[1024];
    int n = read(sockfd, buffer, sizeof(buffer) - 1);
    if (n > 0)
    {
        buffer[n] = '\0';
        handle_server_message(buffer, &sockfd);
    }
    else
    {
        afficher_message(COLOR_RED, "Erreur de communication avec le serveur.");
        exit(EXIT_FAILURE);
    }

    return nom;
}

int main(int argc, char **argv)
{
    signal(SIGINT, sigint_handler);
    if (argc != 3)
    {
        afficher_message(COLOR_RED, "Usage : socket_client <server_ip> <server_port>");
        return EXIT_FAILURE;
    }

    afficher_message(COLOR_BLUE, "Client starting...");

    // Initialisation de la connexion au serveur
    int sockfd = init_connection(argv[1], argv[2]);

    char *nom = malloc(MAX_DESTINATAIRE_SIZE * sizeof(char));

    // Enregistrement du nom de l'utilisateur
    while (nomSet == 0)
    {
        nom = register_user(sockfd);
    }
    // Boucle principale
    event_loop(sockfd, nom);

    free(nom);
    close(sockfd);
    return EXIT_SUCCESS;
}