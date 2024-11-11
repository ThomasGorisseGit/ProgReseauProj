#ifndef PARTIE_H
#define PARTIE_H

#include "awale.h"
#include "server.h"

// Affiche la liste des joueurs disponibles dans le lobby
char* toStringJoueursDispo(Lobby *lobby);

// Envoie un défi à un joueur spécifique par son nom
int defierJoueur(Lobby *lobby, char *nomJoueur, Joueur *joueurSource);

// Initialise la partie Awale
int initialiserPartie(Jeu *jeu);

#endif // PARTIE_H
