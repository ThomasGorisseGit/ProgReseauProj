#ifndef LOBBY_MANAGER_H
#define LOBBY_MANAGER_H
#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <ctype.h>

#include "../donnees/lobby.h"
#include "../donnees/constants.h"

#include "../../vue/echange.h"

Lobby *initialiserLobby();
void fin_partie(Jeu *jeu);

Joueur *trouver_joueur(Lobby *lobby, const char *nom);
void handle_connection(int sockfd, fd_set *readfds, int *max_fd, Lobby *lobby);
Joueur *defier_joueur(Lobby *lobby, char *pseudo);

#endif // LOBBY_MANAGER_H