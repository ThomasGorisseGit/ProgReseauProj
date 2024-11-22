#ifndef COMMANDES_H
#define COMMANDES_H
#include <string.h>
#include "../model/donnees/constants.h"
#include "../model/services/lobbyManager.h"
#include "../model/services/messageManager.h"
#include "../vue/echange.h"
#include "../model/donnees/awale.h"
#include "../model/services/partieManager.h"

void commande_listeJoueurs(Joueur *joueur, Lobby *lobby);
void commande_nom(Joueur *joueur, Lobby *lobby, char body[MAX_BODY_SIZE]);
void commande_message(char destinataire[MAX_DESTINATAIRE_SIZE], char expediteur[MAX_DESTINATAIRE_SIZE], Lobby *lobby, char body[MAX_BODY_SIZE]);
void commande_message_global(Lobby *lobby, char expediteur[MAX_DESTINATAIRE_SIZE], char body[MAX_BODY_SIZE]);
void commande_defier(Lobby *lobby, Joueur *joueur, char destinataire[MAX_DESTINATAIRE_SIZE]);
void commande_declinerDefi(Lobby *lobby, Joueur *joueur, char destinataire[MAX_DESTINATAIRE_SIZE]);
void commande_accepterDefi(Lobby *lobby, Joueur *joueur, char destinataire[MAX_DESTINATAIRE_SIZE]);
void commande_jouerCoup(Lobby *lobby, Joueur *joueur, char body[MAX_BODY_SIZE]);
void commande_modifierBio(Joueur *joueur, char body[MAX_BODY_SIZE]);
void commande_consulterBio(Joueur *joueur, Lobby *lobby, char body[MAX_BODY_SIZE]);
void commande_classement(Joueur *joueur, Lobby *lobby);
void commande_defier_bot(Lobby *lobby, Joueur *joueur);
void commande_deconnexion(Joueur *joueur, Lobby *lobby);
void commande_forfait(Joueur *joueur, Lobby *lobby);
#endif // COMMANDES_H