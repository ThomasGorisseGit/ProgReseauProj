
#ifndef COMMANDES_H
#define COMMANDES_H

#include "../model/donnees/constants.h"
#include "../model/services/lobbyManager.h"
#include "../model/services/messageManager.h"
#include "../vue/echange.h"
#include "../model/donnees/awale.h"
#include "../model/services/partieManager.h"

void commande_listeJoueurs(Joueur *joueur, Lobby *lobby);
void commande_nom(Joueur *joueur, Lobby *lobby, char body[MAX_BODY_SIZE]);
void commande_message(char destinataire[MAX_DESTINATAIRE_SIZE], char expediteur[MAX_DESTINATAIRE_SIZE], Lobby *lobby, char body[MAX_BODY_SIZE]);
void commande_defier(Lobby *lobby, Joueur *joueur, char destinataire[MAX_DESTINATAIRE_SIZE]);
void commande_declinerDefi(Lobby *lobby, Joueur *joueur, char destinataire[MAX_DESTINATAIRE_SIZE]);
void commande_accepterDefi(Lobby *lobby, Joueur *joueur, char destinataire[MAX_DESTINATAIRE_SIZE]);
void commande_jouerCoup(Lobby *lobby, Joueur *joueur, char body[MAX_BODY_SIZE]);

#endif // COMMANDES_H