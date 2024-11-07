#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include "awale.h"
#include <assert.h>
typedef struct {
    int* socket1;
    int* socket2;
} Sockets;



void client_says(int* socket, char* buffer, char* message) {
    
    int n;

    // Envoyer le message au client
    n = write(*socket, message, strlen(message));
    if (n < 0) {
        perror("ERROR writing to socket");
        exit(1);
    }

    // Recevoir la réponse du joueur
    bzero(buffer, 256);  
    n = read(*socket, buffer, 255);
    if (n < 0) {
        perror("ERROR reading from socket");
        exit(1);
    }

    // Ici, buffer contient le nom du joueur saisi
    printf("Le joueur a répondu: %s\n", buffer);
}
int make_a_move(char* buffer, Jeu* jeu){
    int caseDepart;
    int status;
    do{
        Joueur* autre = (jeu->current == jeu->joueur1) ? jeu->joueur2 : jeu->joueur1;

        client_says(jeu->current->socket, buffer, "/coup");  
        caseDepart = atoi(buffer);
        status = jouerCoup(jeu,caseDepart);
        if(status==-1) client_says(jeu->current->socket, buffer, "/icoup");
    } while(status==-1);
    return status;
}
void* handle_game(Sockets* sockets, Jeu* jeu) {
    
   
    char* buffer = malloc(256 * sizeof(char));
    int status;
    while(jeu->vainqueur==NULL) {
        status=make_a_move(buffer, jeu);
        afficherPlateau(jeu);
        if(jeu->vainqueur!=NULL){
            break;
        }
        jeu->current = (jeu->current == jeu->joueur1) ? jeu->joueur2 : jeu->joueur1;
    }

    if (status == 0){
        printf("Egalité\n");
    }
    else {
        printf("Vainqueur : %s\n", jeu->vainqueur->nom);
    }
}

void* start(void *arg) {
    
    Sockets* sockets = (Sockets*)arg;
    int* socket1 = sockets->socket1;
    int* socket2 = sockets->socket2;
    char* buffer = malloc(256 * sizeof(char));


    printf("Initialisation du jeu\n");
    Jeu* jeu = (Jeu*)malloc(sizeof(Jeu));
    Case* plateau = (Case*)malloc(sizeof(Case) * 12);
    Joueur* joueur1 = (Joueur*)malloc(sizeof(Joueur));
    Joueur* joueur2 = (Joueur*)malloc(sizeof(Joueur));
    printf("Initialisation des joueurs\n");
    joueur1->nom = malloc(256 * sizeof(char));
    joueur2->nom = malloc(256 * sizeof(char));
    
    printf("Initialisation des sockets");
    joueur1->socket = socket1;
    joueur2->socket = socket2;

    client_says(socket1, buffer, "/start");
    strcpy(joueur1->nom, buffer);
    printf("Joueur 1 : %s\n", joueur1->nom);

    client_says(socket2, buffer,"/start");
    strcpy(joueur2->nom, buffer);    
    printf("Joueur 2 : %s\n", joueur2->nom);

    
    jeu->plateau = plateau;
    jeu->joueur1 = joueur1;
    jeu->joueur2 = joueur2;
    jeu->vainqueur = NULL;
    jeu->current = joueur1;

    // Initialisation du plateau
    initialiserPlateau(jeu);
    afficherPlateau(jeu);

    printf("début du jeu\n");
    handle_game(sockets, jeu);
    
}

int main(int argc, char** argv) {
    int sockfd, *newsockfd;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;

    if (argc != 2) {
        printf("usage: socket_server port\n");
        exit(0);
    }

    printf("server starting...\n");

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("impossible d'ouvrir le socket\n");
        exit(0);
    }
    printf("socket ok\n");

    bzero((char*)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));
    printf("port : %d\n", atoi(argv[1]));

    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("impossible de faire le bind\n");
        exit(0);
    }
    printf("bind ok\n");

    listen(sockfd, 5);
    printf("listen ok\n");


    int nbClient = 0;
    Sockets sockets;  // Structure pour les sockets
    while (nbClient < 2) {
        clilen = sizeof(cli_addr);
        int* newsockfd = malloc(sizeof(int));
        *newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &clilen);
        if (*newsockfd < 0) {
            perror("ERROR on accept");
            free(newsockfd);
            continue;
        }
        if (nbClient == 0) {
            sockets.socket1 = newsockfd;
        } else if (nbClient == 1) {
            sockets.socket2 = newsockfd;
        }        
        printf("New connection accepted\n");
        nbClient++;
    }

    assert(nbClient == 2);
    // Commence la partie
    pthread_t game_thread;
    pthread_create(&game_thread, NULL, start, (void*)&sockets);
    printf("Game started\n");




    pthread_join(game_thread, NULL);

    // Clean up
    close(sockfd);
    return 1;
}
