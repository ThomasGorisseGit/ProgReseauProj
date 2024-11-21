#ifndef ECHANGE_H
#define ECHANGE_H

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "../model/donnees/constants.h"
#include "../model/donnees/lobby.h"

// Envoyer erreur
int envoyer_erreur(Joueur *joueur, char erreur[MAX_BODY_SIZE]);
int envoyer_message(Joueur *joueur, char *message);
void envoyer_le_joueur_courant(Jeu *jeu);
void demander_case_depart(Joueur *joueur);
void envoyer_gagnant(Jeu *jeu);
void envoyer_egalite(Jeu *jeu);

void envoyer_plateau(Jeu *jeu);
void envoyer_a_tous(Lobby *lobby, char *message);
void envoyer_nom_valide(Joueur *joueur);
void envoyer_nom_invalide(Joueur *joueur);

void envoyer_liste_joueurs(Lobby *lobby, Joueur *joueur);
void envoyer_defi(Joueur *joueur, Joueur *defie);
void envoyer_rejoindre(Joueur *joueur, Lobby *lobby);

void envoyer_decliner_defi(Joueur *joueur, Joueur *demandeur);
void envoyer_accepter_defi(Joueur *joueur, Joueur *demandeur);
void envoyer_bio(Joueur *joueur);
void envoyer_consulter_bio(Joueur *demandeur, Joueur *jbio);

void envoyer_elo_joueurs(Jeu *jeu);
void envoyer_classement(Lobby *lobby, Joueur *joueur);

void afficher_tableau(char *buffer, size_t buffer_size,
                      const char **headers, size_t nb_colonnes,
                      const char *data[][MAX_COLUMNS], size_t nb_lignes);

#endif // ECHANGE_H