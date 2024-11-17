#ifndef AWALE_H
#define AWALE_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "../donnees/constants.h"
#include "../donnees/lobby.h"
void initialiserPlateau(Jeu *jeu);
char *afficherPlateau(Jeu *jeu);
int verifierCasDarret(Jeu *jeu);
void calculerScore(Jeu *jeu, int caseFin);
int jouerCoup(Jeu *jeu, int caseDepart);

#endif // AWALE_H