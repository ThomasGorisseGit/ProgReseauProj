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

// Envoie un défi à un joueur spécifique par son nom
int defierJoueur(Lobby *lobby, char *nomJoueur, Joueur *joueurSource) {
    for (int i = 0; i < lobby->nbJoueurs; i++) {
        Joueur *joueur = lobby->joueurs[i];
        
        if (joueur->status == LOBBY && strcmp(joueur->nom, nomJoueur) == 0) {
            char command[] = "defier";
            char destinataire[100];
            snprintf(destinataire, sizeof(destinataire), "%s", nomJoueur);
            
            ecrire(joueur->socket, command, destinataire); // Envoie la commande de défi formatée
            return 1; // Succès
        }
    }
    return -1; // Joueur non trouvé ou non en LOBBY
}

// Initialise la partie Awale
int initialiserPartie(Jeu *jeu) {
    printf("Initialisation de la partie...\n");
    initialiserPlateau(jeu); // Utilisation de la fonction existante pour initialiser le plateau
    afficherPlateau(jeu);    // Affichage initial du plateau

    printf("La partie est prête à commencer.\n");
    return 1; // Retourne 1 pour indiquer que l'initialisation est réussie
}
