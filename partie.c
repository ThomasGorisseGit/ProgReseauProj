#include "partie.h"
#include "util.h"
#include "server.h"
#include "communication.h"
#include "awale.h"

// Affiche les joueurs en LOBBY

char* toStringJoueursDispo(Lobby *lobby)
{
    char *message = malloc(MAX_MESSAGE_SIZE * sizeof(char));
    strcpy(message, "Pseudo\t\t | \tStatus\n");
    for (int i = 0; i < lobby->nbJoueurs; i++)
    {   Joueur *joueur = lobby->joueurs[i];
        if(joueur->status ==LOBBY) {
        message = strcat(message, lobby->joueurs[i]->nom);
        message = strcat(message, "\t\t | \t");
        message = strcat(message, lobby->joueurs[i]->status == LOBBY ? "Lobby" : "En partie");
        message = strcat(message, "\n");
        }
    }
    return message;
}

// Initialise la partie Awale
int initialiserPartie(Jeu *jeu) {
    printf("Initialisation de la partie...\n");
    initialiserPlateau(jeu); // Utilisation de la fonction existante pour initialiser le plateau
    afficherPlateau(jeu);    // Affichage initial du plateau

    printf("La partie est prête à commencer.\n");
    return 1; // Retourne 1 pour indiquer que l'initialisation est réussie
}
