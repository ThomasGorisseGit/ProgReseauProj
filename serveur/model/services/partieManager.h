#ifndef PARTIE_MANAGER_H
#define PARTIE_MANAGER_H
#include "../donnees/lobby.h"
#include "../../vue/echange.h"
#include "../donnees/awale.h"
#include "lobbyManager.h"
void gerer_coup(Jeu *jeu, Joueur *joueur, int case_depart);
void initialiser_jeu(Lobby *lobby, Joueur *demandeur, Joueur *joueur);
void qui_commence(Jeu *jeu);
#endif // PARTIE_MANAGER_H
