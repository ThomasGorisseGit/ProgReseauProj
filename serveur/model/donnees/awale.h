#ifndef AWALE_H
#define AWALE_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

#include "../donnees/constants.h"
#include "../donnees/lobby.h"
void initialiserPlateau(Jeu *jeu);

int verifierCasDarret(Jeu *jeu);
void calculerScore(Jeu *jeu, int caseFin);
int jouerCoup(Jeu *jeu, int caseDepart);

#endif // AWALE_H