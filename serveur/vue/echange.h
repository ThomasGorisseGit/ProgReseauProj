#ifndef ECHANGE_H
#define ECHANGE_H

#include "../model/donnees/constants.h"
#include "../model/donnees/lobby.h"

// Envoyer erreur
void envoyer_erreur(Joueur *joueur);
int envoyer_message(Joueur *joueur, char *message);
void envoyer_le_joueur_courant(Jeu *jeu);
void demander_case_depart(Joueur *joueur);
void envoyer_gagnant(Jeu *jeu);
void envoyer_egalite(Jeu *jeu);

void envoyer_plateau(Jeu *jeu);

#endif