#ifndef AWALE_H
#define AWALE_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>

typedef struct Joueur {
    char* nom;
    int score;
    int* socket;
} Joueur;

typedef struct Case {
    int nbGraines;
    Joueur* proprietaire;
} Case;


typedef struct Jeu {
    Case* plateau;
    Joueur* joueur1;
    Joueur* joueur2;
    Joueur* vainqueur;
    Joueur* current;
} Jeu;

void initialiserPlateau(Jeu* jeu);
void afficherPlateau(Jeu* jeu);
int verifierCasDarret(Jeu* jeu);
void calculerScore(Jeu* jeu, int caseFin);
int jouerCoup(Jeu* jeu, int caseDepart);


#endif // AWALE_H