#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>

#include "../model/constants.h"

void afficher_guide();
void afficher_message(char *color, char message[MAX_MESSAGE_SIZE]);

#endif // DISPLAY_H