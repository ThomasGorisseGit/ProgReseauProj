#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_COMMAND_SIZE 50
#define MAX_DESTINATAIRE_SIZE 50
#define MAX_BODY_SIZE 150

int verifierFormatMessage(char *message, char *command, char *destinataire, char *body, char *expediteur);
void ecrire(int *socket, char *command, char *destinataire, char *body, char *expediteur);

#endif