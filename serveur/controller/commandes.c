#include "commandes.h"

void commande_listeJoueurs(Joueur *joueur, Lobby *lobby)
{
    envoyer_liste_joueurs(lobby, joueur);
}

void commande_nom(Joueur *joueur, Lobby *lobby, char body[MAX_BODY_SIZE])
{
    strcpy(joueur->nom, body);
    joueur->status = LOBBY;
    envoyer_rejoindre(joueur, lobby);
    usleep(2000);
    envoyer_liste_joueurs(lobby, joueur);
}

void commande_message(char destinataire[MAX_DESTINATAIRE_SIZE], char expediteur[MAX_DESTINATAIRE_SIZE], Lobby *lobby, char body[MAX_BODY_SIZE])
{
    Joueur *destinataireJoueur = trouver_joueur(lobby, destinataire);
    if (destinataireJoueur != NULL)
    {
        // Format the message to be sent to the target player
        char msg[MAX_MESSAGE_SIZE];
        snprintf(msg, sizeof(msg), "/message #%s #%s %s", expediteur, destinataire, body);
        envoyer_message(destinataireJoueur, msg);
        printf("Message envoyé à %s de la part de %s: %s\n", destinataire, expediteur, body);
    }
    else
    {
        // Send an error message back to the sender if the recipient does not exist
        char error_msg[MAX_MESSAGE_SIZE];
        snprintf(error_msg, sizeof(error_msg), "/message #server #%s Le joueur %s n'existe pas ou n'est pas connecté.", expediteur, destinataire);
        Joueur *j_expediteur = trouver_joueur(lobby, expediteur);
        envoyer_message(j_expediteur, error_msg);
        printf("Erreur : Le joueur %s n'existe pas ou n'est pas connecté.\n", destinataire);
    }
}

void commande_defier(Lobby *lobby, Joueur *joueur, char destinataire[MAX_DESTINATAIRE_SIZE])
{
    Joueur *defie = defier_joueur(lobby, destinataire);
    if (strcmp(joueur->nom, destinataire) == 0)
    {
        envoyer_erreur(joueur); // Vous ne pouvez pas vous défier vous-même.
    }
    else if (defie != NULL)
    {
        defie->status = DEFI;
        joueur->status = DEFI;
        envoyer_defi(joueur, defie);
    }
    else
    {
        envoyer_erreur(joueur); // Le joueur %s n'est pas disponible pour un défi.
    }
}

void commande_declinerDefi(Lobby *lobby, Joueur *joueur, char destinataire[MAX_DESTINATAIRE_SIZE])
{
    Joueur *demandeur = trouver_joueur(lobby, destinataire);
    if (demandeur == NULL)
    {
        envoyer_erreur(joueur);
    }
    else
    {
        envoyer_decliner_defi(joueur, demandeur);
        demandeur->status = LOBBY;
        joueur->status = LOBBY;
    }
}

void commande_accepterDefi(Lobby *lobby, Joueur *joueur, char destinataire[MAX_DESTINATAIRE_SIZE])
{
    Joueur *demandeur = trouver_joueur(lobby, destinataire);
    if (demandeur == NULL)
    {
        envoyer_erreur(joueur); // Le joueur %s n'existe pas ou n'est pas connecté.
    }
    envoyer_accepter_defi(joueur, demandeur);

    // Initialiser la partie
    initialiser_jeu(lobby, demandeur, joueur);

    Jeu *jeu = lobby->jeux[joueur->idPartie];
    if (jeu == NULL)
    {
        perror("Erreur d'allocation mémoire pour le jeu");
        exit(1);
    }

    // récupération du plateau de jeu
    char *string_plateau = afficherPlateau(jeu);
    envoyer_plateau(jeu, string_plateau);
    envoyer_le_joueur_courant(jeu);
    demander_case_depart(jeu->current);
}

void commande_jouerCoup(Lobby *lobby, Joueur *joueur, char body[MAX_BODY_SIZE])
{
    int case_depart = atoi(body);
    gerer_coup(lobby->jeux[joueur->idPartie], joueur, case_depart);
}