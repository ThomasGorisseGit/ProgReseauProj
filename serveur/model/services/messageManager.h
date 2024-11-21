#ifndef messageManager_h
#define messageManager_h
#include <stdio.h>
#include <stddef.h>
#include <sys/select.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../donnees/lobby.h"
#include "../donnees/constants.h"

// verifier le format d'un message
int verifierFormatMessage(char *message, char *command, char *destinataire, char *expediteur, char *body);

#endif // messageManager_h