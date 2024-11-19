#ifndef MESSAGE_MANAGER_H
#define MESSAGE_MANAGER_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "constants.h"

void ecrire(int *socket, char *command, char *expediteur, char *destinataire, char *body);
int verifierFormatMessage(char *message, char *command, char *expediteur, char *destinataire, char *body);
#endif // MESSAGE_MANAGER_H