#include "partie.h"
#include "util.h"
#include "server.h"
#include "communication.h"
#include "awale.h"

// Affiche les joueurs en LOBBY

char *toStringJoueursDispo(Lobby *lobby)
{
    char *message = malloc(MAX_MESSAGE_SIZE * sizeof(char));
    strcpy(message, "Pseudo\t\t | \tStatus\n");
    for (int i = 0; i < lobby->nbJoueurs; i++)
    {
        Joueur *joueur = lobby->joueurs[i];
        if (joueur->status == LOBBY)
        {
            message = strcat(message, lobby->joueurs[i]->nom);
            message = strcat(message, "\t\t | \t");
            message = strcat(message, lobby->joueurs[i]->status == LOBBY ? "Lobby" : "En partie");
            message = strcat(message, "\n");
        }
    }
    return message;
}

#include <string.h>

Joueur *defierJoueur(Lobby *lobby, char *pseudo)
{
    for (int i = 0; i < lobby->nbJoueurs; i++)
    {
        Joueur *joueur = lobby->joueurs[i];

        // Vérifie si le pseudo correspond
        if (strcmp(joueur->nom, pseudo) == 0)
        {
            // Vérifie si le joueur est dans le lobby et disponible
            if (joueur->status == LOBBY)
            {
                return joueur; // Retourne le pointeur vers le joueur disponible
            }
            else
            {
                return NULL; // Le joueur est en partie
            }
        }
    }
    return NULL;
}

// Initialise la partie Awale
char *initialiserPartie(Jeu *jeu)
{
    printf("Initialisation de la partie...\n");
    initialiserPlateau(jeu); // Utilisation de la fonction existante pour initialiser le plateau
    printf("Plateau initialisé\n\n");
    char *plateau = afficherPlateau(jeu);

    printf("La partie est prête à commencer.\n");
    return plateau; // Retourne 1 pour indiquer que l'initialisation est réussie
}

// Fonction pour trouver un joueur par son nom dans le lobby
Joueur *trouverJoueurParNom(Lobby *lobby, const char *nom)
{
    for (int i = 0; i < lobby->nbJoueurs; i++)
    {
        if (strcmp(lobby->joueurs[i]->nom, nom) == 0) // Comparaison des noms
        {
            return lobby->joueurs[i]; // Retourne le joueur trouvé
        }
    }
    return NULL; // Retourne NULL si le joueur n'est pas trouvé
}