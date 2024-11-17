#ifndef ECHANGE_H
#define ECHANGE_H

#include "../model/donnees/constants.h"
#include "../model/donnees/lobby.h"

// Envoyer erreur
void envoyer_erreur(Joueur *joueur);
int envoyer_message(Joueur *joueur, char *message);

#endif