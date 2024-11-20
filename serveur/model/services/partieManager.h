#ifndef PARTIE_MANAGER_H
#define PARTIE_MANAGER_H
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "../donnees/lobby.h"
#include "../../vue/echange.h"
#include "../donnees/awale.h"
#include "lobbyManager.h"

void initialiser_jeu(Lobby *lobby, Joueur *demandeur, Joueur *joueur);
void qui_commence(Jeu *jeu);
#endif // PARTIE_MANAGER_H
