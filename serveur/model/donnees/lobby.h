#ifndef LOBBY_H
#define LOBBY_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
typedef enum
{
    INIT,
    LOBBY,
    PARTIE,
    DEFI
} Status;
typedef enum
{
    HUMAIN,
    IA
} Type;
typedef struct Joueur
{
    char *nom;
    int score;
    int *socket;
    Status status;
    int idPartie;
    int idJoueur;
    char *bio;
    double elo;
    Type type;

} Joueur;

typedef struct Case
{
    int nbGraines;
    Joueur *proprietaire;
} Case;

typedef struct Jeu
{
    Case *plateau;
    Joueur *joueur1;
    Joueur *joueur2;
    Joueur *vainqueur;
    Joueur *current;
} Jeu;

typedef struct
{
    Joueur **joueurs;
    int nbJoueurs;
    Jeu **jeux;
    int nbJeux;
} Lobby;

#endif