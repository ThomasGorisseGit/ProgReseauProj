#ifndef LOBBY_H
#define LOBBY_H
#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <ctype.h>

#include "../donnees/lobby.h"
#include "../donnees/constants.h"

Lobby *initialiserLobby();
void fin_partie(Jeu *jeu);

Joueur *trouver_joueur(Lobby *lobby, const char *nom);
void handle_connection(int sockfd, fd_set *readfds, int *max_fd, Lobby *lobby);
Joueur *defier_joueur(Lobby *lobby, char *pseudo);

int commande_listeJoueurs(Joueur *joueur, Lobby *lobby);
int commande_nom(Joueur *joueur, Lobby *lobby, char body[MAX_BODY_SIZE]);
int commande_message(char destinataire[MAX_DESTINATAIRE_SIZE], Joueur *expediteur, Lobby *lobby, char body[MAX_BODY_SIZE]);
int commande_defier(Lobby *lobby, Joueur *joueur, char destinataire[MAX_DESTINATAIRE_SIZE]);
int commande_declinerDefi(Lobby *lobby, Joueur *joueur, char destinataire[MAX_DESTINATAIRE_SIZE]);

#endif // LOBBY_H