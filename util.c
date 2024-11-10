#include "util.h"

int verifierFormatMessage(char *message, char *command, char *destinataire, char *body)
{
    // Nettoyer le message
    size_t len = strlen(message);
    while (len > 0 && (message[len - 1] == '\n' || message[len - 1] == ' '))
    {
        message[len - 1] = '\0';
        len--;
    }

    // Utiliser strtok pour diviser le message
    char *token = strtok(message, " "); // On sépare au premier espace
    if (token == NULL || token[0] != '/')
    {
        return 0; // Mauvais format de commande
    }

    strcpy(command, token + 1); // Enlever le '/' et copier la commande

    token = strtok(NULL, " "); // Le destinataire
    if (token == NULL || token[0] != '#')
    {
        return 0; // Mauvais format de destinataire
    }

    strcpy(destinataire, token + 1); // Enlever le '#' et copier le destinataire

    token = strtok(NULL, ""); // Reste du message (body)
    if (token == NULL)
    {
        return 0; // Pas de message (body) trouvé
    }

    strcpy(body, token); // Copier le corps du message

    return 1; // Succès
}

void ecrire(int *socket, char *command, char *destinataire)
{
    char body[MAX_BODY_SIZE];
    char message[MAX_BODY_SIZE + MAX_COMMAND_SIZE + MAX_DESTINATAIRE_SIZE + 3];
    int n;

    bzero(body, MAX_BODY_SIZE);
    fgets(body, MAX_BODY_SIZE - 1, stdin);

    sprintf(message, "/%s #%s %s", command, destinataire, body);
    n = write(*socket, message, strlen(message));
    printf("Message envoyé:%s", message);
    if (n < 0)
    {
        perror("ERROR writing to socket");
        exit(1);
    }
}