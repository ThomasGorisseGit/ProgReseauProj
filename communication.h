#ifndef COMMUNICATION_H
#define COMMUNICATION_H
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "awale.h"
#include "server.h"

int envoyer(Joueur *joueur, char *message);
void envoyerATousDansLobby(Lobby *lobby, char *message);

#endif // COMMUNICATION_H
