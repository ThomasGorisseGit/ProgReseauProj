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

void handle_connection(int sockfd, fd_set *readfds, int *max_fd, Lobby *lobby);
#endif // LOBBY_H