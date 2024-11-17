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
        strcpy(joueur->nom, body);
        joueur->status = LOBBY;
        envoyer_rejoindre(joueur, lobby);
        usleep(2000);
        envoyer_liste_joueurs(lobby, joueur);
    }

    else if (strcmp(command, "listeJoueurs") == 0)
    {
        envoyer_liste_joueurs(lobby, joueur);
    }

    else if (strcmp(command, "defier") == 0)
    {
        Joueur *defie = defier_joueur(lobby, destinataire);
        if (strcmp(joueur->nom, destinataire) == 0)
        {
            envoyer_erreur(joueur); // Vous ne pouvez pas vous défier vous-même.
        }
        else if (defie != NULL)
        {
            defie->status = DEFI;
            joueur->status = DEFI;
            envoyer_defi(joueur, defie);
        }
        else
        {
            envoyer_erreur(joueur); // Le joueur %s n'est pas disponible pour un défi.
        }
    }

    else if (strcmp(command, "declinerDefi") == 0)
    {
        Joueur *demandeur = trouver_joueur(lobby, destinataire);
        if (demandeur != NULL)
        {
            envoyer_decliner_defi(joueur, demandeur);
            demandeur->status = LOBBY;
            joueur->status = LOBBY;
        }
    }

    else if (strcmp(command, "accepterDefi") == 0)
    {
        Joueur *demandeur = trouver_joueur(lobby, destinataire);
        if (demandeur == NULL)
        {
            envoyer_erreur(joueur); // Le joueur %s n'existe pas ou n'est pas connecté.
        }
        envoyer_accepter_defi(joueur, demandeur);
        // Initialiser la partie
        initialiser_jeu(lobby, demandeur, joueur);

        Jeu *jeu = lobby->jeux[joueur->idPartie];
        if (jeu == NULL)
        {
            perror("Erreur d'allocation mémoire pour le jeu");
            exit(1);
        }

        envoyer_plateau(jeu);
        envoyer_le_joueur_courant(jeu);
        demander_case_depart(jeu->current);
    }

    else if (strcmp(command, "message") == 0)
    {
        commande_message(destinataire, expediteur, lobby, body);
    }
    else if (strcmp(command, "coup") == 0)
    {
        int case_depart = atoi(body);
        gerer_coup(lobby->jeux[joueur->idPartie], joueur, case_depart);
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
            printf("Message reçu de %s: %s\n", joueur->nom, message);
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