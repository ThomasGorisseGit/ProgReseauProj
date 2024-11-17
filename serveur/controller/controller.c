#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>
#include "../model/donnees/constants.h"
#include "../model/services/lobbyManager.h"
#include "../model/services/messageManager.h"
#include "../vue/echange.h"
Lobby *lobby;

void message_handler(Joueur *joueur, char *message)
{
    char command[MAX_COMMAND_SIZE], destinataire[MAX_DESTINATAIRE_SIZE], body[MAX_BODY_SIZE], expediteur[MAX_DESTINATAIRE_SIZE];
    int valid_message = verifierFormatMessage(message, command, expediteur, destinataire, body);
    if (!valid_message)
    {
        envoyer_erreur(joueur);
        return;
    }
    if (strcmp(command, "name") == 0)
    {
        strcpy(joueur->nom, body);
        joueur->status = LOBBY;
        printf("Nom du joueur %d mis à jour : %s\n", joueur->idJoueur, joueur->nom);

        // Envoi du message "joining" proprement formaté
        char join_msg[MAX_MESSAGE_SIZE];
        snprintf(join_msg, sizeof(join_msg), "/joining #server #%s Un nouveau joueur a rejoint le lobby : %s\n", joueur->nom, joueur->nom);
        envoyerATousDansLobby(lobby, join_msg);
        usleep(2000);
        char lobby_msg[MAX_MESSAGE_SIZE];
        snprintf(lobby_msg, sizeof(lobby_msg), "/displayLobby #server #%s %s", joueur->nom, toStringLobby(lobby));
        envoyer(joueur, lobby_msg);
    }

    else if (strcmp(command, "listeJoueurs") == 0)
    {
        char liste_message[MAX_MESSAGE_SIZE];
        snprintf(liste_message, sizeof(liste_message), "/listeJoueurs #server #%s %s", joueur->nom, toStringLobby(lobby));
        envoyer(joueur, liste_message);
    }

    else if (strcmp(command, "defier") == 0)
    {
        Joueur *defie = defierJoueur(lobby, destinataire);
        if (strcmp(joueur->nom, destinataire) == 0)
        {
            char defier_msg[MAX_MESSAGE_SIZE];
            snprintf(defier_msg, sizeof(defier_msg), "/message #%s #%s Vous ne pouvez pas vous défier vous-même.", joueur->nom, joueur->nom);
            envoyer(joueur, defier_msg);
        }
        else if (defie != NULL)
        {
            defie->status = DEFI;
            joueur->status = DEFI;
            char defier_msg[MAX_MESSAGE_SIZE];
            snprintf(defier_msg, sizeof(defier_msg), "/defier #%s #%s Le joueur %s veut vous défier. Pour accepter, tapez 1 ; pour refuser, tapez 0.", defie->nom, joueur->nom, joueur->nom);
            envoyer(defie, defier_msg);
        }
        else
        {
            char error_msg[MAX_MESSAGE_SIZE];
            snprintf(error_msg, sizeof(error_msg), "/message #%s #serveur Le joueur %s n'est pas disponible pour un défi.", joueur->nom, destinataire);
            envoyer(joueur, error_msg);
        }
    }

    else if (strcmp(command, "declinerDefi") == 0)
    {
        Joueur *demandeur = trouverJoueurParNom(lobby, destinataire);
        if (demandeur != NULL)
        {
            char msg[MAX_MESSAGE_SIZE];
            snprintf(msg, sizeof(msg), "/message #%s #server %s a refusé votre défi.", demandeur->nom, joueur->nom);
            envoyer(demandeur, msg);
            demandeur->status = LOBBY;
            joueur->status = LOBBY;
        }
    }

    else if (strcmp(command, "accepterDefi") == 0)
    {
        Joueur *demandeur = trouverJoueurParNom(lobby, destinataire);
        if (demandeur != NULL)
        {
            char msg[MAX_MESSAGE_SIZE];
            snprintf(msg, sizeof(msg), "/message #%s #server %s a accepté votre défi.", demandeur->nom, joueur->nom);
            envoyer(demandeur, msg);

            demandeur->status = PARTIE;
            joueur->status = PARTIE;

            // Initialiser la partie
            Jeu *jeu = malloc(sizeof(Jeu));
            // vérification de l'existance du jeu
            if (jeu == NULL)
            {
                perror("Erreur d'allocation mémoire pour le jeu");
                exit(1);
            }
            jeu->joueur1 = demandeur;
            jeu->joueur2 = joueur;
            jeu->current = demandeur;
            jeu->joueur1 = demandeur;
            jeu->joueur2 = joueur;

            if (jeu->joueur1 == NULL || jeu->joueur2 == NULL || jeu->current == NULL)
            {
                fprintf(stderr, "Les joueurs ne sont pas correctement initialisés.\n");
                free(jeu); // Libération de la mémoire pour éviter une fuite
                exit(1);
            }

            joueur->idPartie = lobby->nbJeux;
            demandeur->idPartie = lobby->nbJeux;

            lobby->jeux[lobby->nbJeux] = jeu;
            printf("Joueur 1: %s, Joueur 2: %s\n", jeu->joueur1->nom, jeu->joueur2->nom);
            char *plateau_msg = malloc(sizeof(char) * 1024);
            plateau_msg = initialiserPartie(lobby->jeux[joueur->idPartie]);
            snprintf(plateau_msg, sizeof(char) * 1024, "/message #server #%s Plateau initial :\n %s\n", demandeur->nom, plateau_msg);
            envoyerAuxJoueurs(lobby->jeux[joueur->idPartie], plateau_msg);
            lobby->nbJeux++;
        }
    }
    else if (strcmp(command, "message") == 0)
    {
        // Find the target player by their name
        Joueur *destinataireJoueur = trouverJoueurParNom(lobby, destinataire);
        if (destinataireJoueur != NULL)
        {
            // Format the message to be sent to the target player
            char msg[MAX_MESSAGE_SIZE];
            snprintf(msg, sizeof(msg), "/message #%s #%s %s", destinataire, joueur->nom, body);
            envoyer(destinataireJoueur, msg);
            printf("Message envoyé à %s de la part de %s: %s\n", destinataire, joueur->nom, body);
        }
        else
        {
            // Send an error message back to the sender if the recipient does not exist
            char error_msg[MAX_MESSAGE_SIZE];
            snprintf(error_msg, sizeof(error_msg), "/message #server #%s Le joueur %s n'existe pas ou n'est pas connecté.", joueur->nom, destinataire);
            envoyer(joueur, error_msg);
            printf("Erreur : Le joueur %s n'existe pas ou n'est pas connecté.\n", destinataire);
        }
    }

    else
    {
        printf("Commande inconnue reçue: %s\n", command);
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