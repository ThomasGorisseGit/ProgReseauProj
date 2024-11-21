#include "display.h"
void afficher_guide()
{
    printf(COLOR_YELLOW "Commandes disponibles:\n" COLOR_RESET);
    printf(COLOR_GREEN "/listeJoueurs" COLOR_RESET " - Liste les joueurs dans le lobby\n");
    printf(COLOR_GREEN "/defier <nom>" COLOR_RESET " - Défi un joueur spécifié par son nom\n");
    printf(COLOR_GREEN "/message <nom> : <message>" COLOR_RESET " - Envoyer un message à un joueur\n");
    printf(COLOR_GREEN "/modifierBio <message>" COLOR_RESET " - Modifier la description de votre profil\n");
    printf(COLOR_GREEN "/consulterBio <nom>" COLOR_RESET " - Consulter la description d'un joueur\n");
    printf(COLOR_GREEN "/classement" COLOR_RESET " - Consulter le classement des meilleurs joueurs\n");
}

void afficher_message(char *color, char message[MAX_BODY_SIZE])
{
    if (color == NULL || strcmp(color, "") == 0)
    {
        color = COLOR_RESET; // Utiliser COLOR_RESET si aucune couleur n'est spécifiée
    }
    printf("%s%s%s\n", color, message, COLOR_RESET);
}

void afficher_message_nocolour(char message[MAX_BODY_SIZE])
{
    printf("%s\n", message);
}