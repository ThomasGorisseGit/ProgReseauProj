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

void ecrire(int *socket)
{
  char buffer[256];
  int n;

  printf("Saisie : ");
  bzero(buffer, 256);
  fgets(buffer, 255, stdin);
  n = write(*socket, buffer, strlen(buffer));
  if (n < 0)
  {
    perror("ERROR writing to socket");
    exit(1);
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

  /*
   *  partie client
   */
  printf("client starting\n");

  /* initialise la structure de donnee */
  bzero((char *)&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
  serv_addr.sin_port = htons(atoi(argv[2]));

  /* ouvre le socket */
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    printf("socket error\n");
    exit(0);
  }

  /* effectue la connection */
  if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
  {
    printf("socket error\n");
    exit(0);
  }

  while (1)
  {
    char *buffer = malloc(256 * sizeof(char));
    int n = read(sockfd, buffer, 256);
    if (n < 0)
    {
      printf("Error reading from socket\n");
      continue;
    }
    if (strcmp(buffer, "") != 0)
    {
      if (strcmp(buffer, "/start") == 0)
      {
        printf("Quel est votre nom ?\n");
        ecrire(&sockfd);
      }
      else if (strcmp(buffer, "/coup") == 0)
      {
        printf("Saisir le coup\n");
        ecrire(&sockfd);

      }
      else if (strcmp(buffer, "/stop") == 0)
      {
        printf("Fin de la partie\n");
      }
      else if (strcmp(buffer, "/icoup") == 0)
      {
        printf("Coup invalide\n");
        continue;
      }
      else
      {
        printf("Received: %s\n", buffer);
      }

    }
    free(buffer);
  }

  return 1;
}
