#ifndef CLI_CONTROLLER_H
#define CLI_CONTROLLER_H

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
#include "../vue/display.h"

int main(int argc, char **argv);
#endif // CLI_CONTROLLER_H