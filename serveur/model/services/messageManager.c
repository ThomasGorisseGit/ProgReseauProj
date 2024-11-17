#include "messageManager.h"

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

int check_if_message(Lobby *lobby, fd_set *readfds)
{
    for (int i = 0; i < lobby->nbJoueurs; i++)
    {
        Joueur *joueur = lobby->joueurs[i];
        char message[MAX_MESSAGE_SIZE];
        int n;

        if (FD_ISSET(*joueur->socket, readfds))
        {
            n = read(*joueur->socket, message, MAX_MESSAGE_SIZE);
            if (n < 0)
            {
                perror("Erreur de lecture du socket");
                continue;
            }
            if (n == 0)
            {
                printf("Client déconnecté\n");
                close(*joueur->socket);
                FD_CLR(*joueur->socket, readfds);
                free(joueur->nom);
                free(joueur);
                lobby->joueurs[i] = lobby->joueurs[--lobby->nbJoueurs];
                i--;
                continue;
            }

            message[n] = '\0';
            printf("Message reçu de %s: %s\n", joueur->nom, message);
            message_handler(joueur, message);
        }
    }

    return 0;
}
