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
#include "../model/messageManager.h"

int main(int argc, char **argv);

void handle_server_message(char *message, int *sockfd);
void handle_client_input(char *user_input, int *sockfd, char *nom);
int init_connection(const char *server_ip, const char *server_port);
void event_loop(int sockfd, char *nom);
char *register_user(int sockfd);

#endif // CLI_CONTROLLER_H