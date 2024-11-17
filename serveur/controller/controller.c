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
        commande_message(destinataire, expediteur, lobby, body);
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