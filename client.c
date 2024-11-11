/* Client pour les sockets
 *    socket_client ip_server port
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "communication.h"

#include "util.h"

void handle_message(char *message, int *sockfd)
{
  char command[50], destinataire[50], body[150];
  if (verifierFormatMessage(message, command, destinataire, body))
  {
    if (strcmp(command, "name") == 0)
    {
      printf("%s\n", body);
      ecrire(sockfd, command, destinataire);
    }
    else if (strcmp(command, "joining") == 0)
    {
      printf("%s\n", body);
    }
    else if (strcmp(command, "displayLobby") == 0)
    {
      printf("%s\n", body);
      printf("Ecrivez ce que vouis voulez faire");
      ecrire(sockfd, "listeJoueurs", "server");
    }
    else if (strcmp(command, "defi") == 0)
    {
      ecrire(sockfd, command, destinataire);
    }
    else if(strcmp(command, "listeJoueurs") == 0){
      printf("%s\n", body);
    } else
    {
      printf("Commande inconnue\n");
      printf("Commandes possibles : name, joining, displayLobby, defi\n");
    }
  }
  else
  {
    printf("Message invalide. Format attendu : /command #destinataire messageEnvoyé\n");
  }
}

int main(int argc, char **argv)
{
  int sockfd, newsockfd, clilen, chilpid, ok, nleft, nbwriten;
  char c;
  struct sockaddr_in cli_addr, serv_addr;

  if (argc != 3)
  {
    printf("usage  socket_client server port\n");
    exit(0);
  }
  printf("client starting\n");

  bzero((char *)&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
  serv_addr.sin_port = htons(atoi(argv[2]));
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    printf("socket error\n");
    exit(0);
  }

  if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
  {
    printf("socket error\n");
    exit(0);
  }

  while (1)
  {
    char *buffer = malloc(1024 * sizeof(char));
    int n = read(sockfd, buffer, 1024);
    if (n < 0)
    {
      printf("Error reading from socket\n");
      continue;
    }
    if (strcmp(buffer, "") != 0)
    {
      handle_message(buffer, &sockfd);
    }
    free(buffer);
  }

  return 1;
}
