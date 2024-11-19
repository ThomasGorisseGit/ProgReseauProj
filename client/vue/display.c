#include "display.h"
void afficher_guide()
{
    printf(COLOR_YELLOW "Commandes disponibles:\n" COLOR_RESET);
    printf(COLOR_GREEN "/listeJoueurs" COLOR_RESET " - Liste les joueurs dans le lobby\n");
    printf(COLOR_GREEN "/defier <nom>" COLOR_RESET " - Défi un joueur spécifié par son nom\n");
    printf(COLOR_GREEN "/message <nom> : <message>" COLOR_RESET " - Envoyer un message à un joueur\n");
}