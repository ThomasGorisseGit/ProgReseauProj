#include "messageManager.h"

void ecrire(int *socket, char *command, char *expediteur, char *destinataire, char *body)
{
    char message[MAX_BODY_SIZE + MAX_COMMAND_SIZE + MAX_DESTINATAIRE_SIZE + MAX_DESTINATAIRE_SIZE + 4];
    int n;

    // Construire le message avec l'expéditeur inclus
    sprintf(message, "/%s #%s #%s %s", command, expediteur, destinataire, body);
    n = write(*socket, message, strlen(message));
    printf("Message envoyé: %s\n", message);
    if (n < 0)
    {
        perror("Erreur d'écriture sur le socket");
        exit(1);
    }
}
int verifierFormatMessage(char *message, char *command, char *expediteur, char *destinataire, char *body)
{
    size_t len = strlen(message);
    while (len > 0 && (message[len - 1] == '\n' || message[len - 1] == ' '))
    {
        message[len - 1] = '\0';
        len--;
    }

    char *token = strtok(message, " ");
    if (token == NULL || token[0] != '/')
    {
        return 0;
    }
    strcpy(command, token + 1);

    token = strtok(NULL, " ");
    if (token == NULL || token[0] != '#')
    {
        return 0;
    }
    strcpy(expediteur, token + 1);

    token = strtok(NULL, " ");
    if (token == NULL || token[0] != '#')
    {
        return 0;
    }
    strcpy(destinataire, token + 1);

    token = strtok(NULL, "");
    if (token == NULL)
    {
        body[0] = '\0';
    }
    else
    {
        strcpy(body, token);
    }

    return 1;
}