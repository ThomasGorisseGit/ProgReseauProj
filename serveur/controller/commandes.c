#include "commandes.h"

void commande_listeJoueurs(Joueur *joueur, Lobby *lobby)
{
    envoyer_liste_joueurs(lobby, joueur);
}

void commande_nom(Joueur *joueur, Lobby *lobby, char body[MAX_BODY_SIZE])
{
    int used = 0;
    for (int i = 0; i < lobby->nbJoueurs; i++)
    {
        if (strcmp(lobby->joueurs[i]->nom, body) == 0)
        {
            used = 1;
            break;
        }
    }
    if (used == 0)
    {
        strcpy(joueur->nom, body);
        joueur->status = LOBBY;
        envoyer_nom_valide(joueur);
        usleep(2000);
        envoyer_rejoindre(joueur, lobby);

        usleep(2000);
        envoyer_liste_joueurs(lobby, joueur);
    }
    else
    {
        envoyer_nom_invalide(joueur);
    }
}
void commande_deconnexion(Joueur *joueur, Lobby *lobby)
{
    // Construire le message de déconnexion
    char message[MAX_MESSAGE_SIZE];
    snprintf(message, sizeof(message), "/message #server #%s %s Le joueur %s s'est déconnecté.%s\n",
             joueur->nom, COLOR_RED, joueur->nom, COLOR_RESET);

    // Envoyer le message à tous les joueurs du lobby
    envoyer_a_tous(lobby, message);
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
        snprintf(error_msg, sizeof(error_msg), "/message #server #%s Le joueur %s n'existe pas ou n'est pas disponible.", expediteur, destinataire);
        Joueur *j_expediteur = trouver_joueur(lobby, expediteur);
        envoyer_message(j_expediteur, error_msg);
        printf("Erreur : Le joueur %s n'existe pas ou n'est pas connecté.\n", destinataire);
    }
}

void commande_message_global(Lobby *lobby, char expediteur[MAX_DESTINATAIRE_SIZE], char body[MAX_BODY_SIZE])
{
    Joueur *expediteurJoueur = trouver_joueur(lobby, expediteur);

    envoyer_message_global(expediteurJoueur, lobby, body);

    printf("Message global envoyé par %s à tous les joueurs sauf lui-même : %s\n", expediteur, body);
}

void commande_defier(Lobby *lobby, Joueur *joueur, char destinataire[MAX_DESTINATAIRE_SIZE])
{
    Joueur *defie = defier_joueur(lobby, destinataire);
    if (strcmp(joueur->nom, destinataire) == 0)
    {
        envoyer_erreur(joueur, "Vous ne pouvez pas vous défier vous même !"); // Vous ne pouvez pas vous défier vous-même.
    }
    else if (defie != NULL)
    {
        defie->status = DEFI;
        joueur->status = DEFI;
        envoyer_defi(joueur, defie);
    }
    else
    {
        envoyer_erreur(joueur, "Le joueur n'est pas disponible pour un défi"); // Le joueur %s n'est pas disponible pour un défi.
    }
}

void commande_declinerDefi(Lobby *lobby, Joueur *joueur, char destinataire[MAX_DESTINATAIRE_SIZE])
{
    Joueur *demandeur = trouver_joueur(lobby, destinataire);
    if (demandeur == NULL)
    {
        envoyer_erreur(joueur, "Le joueur n'existe pas ou n'est pas connecté."); // Le joueur %s n'existe pas ou n'est pas connecté.
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
    printf("Recherche joueur");
    Joueur *demandeur = trouver_joueur(lobby, destinataire);
    if (demandeur == NULL)
    {
        envoyer_erreur(joueur, "Le joueur n'existe pas ou n'est pas connecté"); // Le joueur %s n'existe pas ou n'est pas connecté.
    }
    printf("Joueur trouvé%s", demandeur->nom);
    envoyer_accepter_defi(joueur, demandeur);
    usleep(2000);

    // // Initialiser la partie
    initialiser_jeu(lobby, demandeur, joueur);

    Jeu *jeu = lobby->jeux[joueur->idPartie];
    if (jeu == NULL)
    {
        perror("Erreur d'allocation mémoire pour le jeu");
        exit(1);
    }

    envoyer_plateau(jeu);
    usleep(2000);

    envoyer_le_joueur_courant(jeu);
    usleep(2000);

    demander_case_depart(jeu->current);
}

void commande_jouerCoup(Lobby *lobby, Joueur *joueur, char body[MAX_BODY_SIZE])
{
    int case_depart = atoi(body);
    Jeu *jeu = lobby->jeux[joueur->idPartie];
    if (joueur->idPartie == -1)
    {
        printf("erreur id partie");
        envoyer_erreur(joueur, "Erreur lors de la récupération de l'id de la partie");
        return;
    }
    if (joueur->status != PARTIE)
    {
        printf("erreur not in partie");
        envoyer_erreur(joueur, "Vous n'êtes pas en partie");
        return;
    }
    if (jeu->current->nom != joueur->nom)
    {
        printf("Ce n'est pas a ce joueur de jouer");
        envoyer_erreur(joueur, "Ce n'est pas à vous de jouer");
        return;
    }
    int coups_valide = jouerCoup(jeu, case_depart);
    if (coups_valide == -1)
    {
        // Le coups est invalide, on redemande a l'utilisateur
        envoyer_erreur(joueur, "Coup invalide, veuillez rejouer");
        usleep(2000);

        demander_case_depart(joueur);
        return;
    }
    if (coups_valide == 0)
    {
        // égalité
        envoyer_egalite(jeu);
        usleep(2000);
        calculerElo(jeu->joueur1, jeu->joueur2, 0.5);
        fin_partie(jeu);
        return;
    }

    if (jeu->vainqueur != NULL && (jeu->vainqueur->nom == jeu->joueur1->nom || jeu->vainqueur->nom == jeu->joueur2->nom))
    {
        // On a un vainqueur
        printf("Vainqueur : %s", jeu->vainqueur->nom);
        envoyer_gagnant(jeu);
        usleep(2000);

        // On récupère l'élo des joueurs.
        Joueur *perdant = (jeu->joueur1 == jeu->vainqueur) ? jeu->joueur2 : jeu->joueur1;
        calculerElo(jeu->vainqueur, perdant, 1);
        envoyer_elo_joueurs(jeu);

        usleep(2000);
        fin_partie(jeu);
        return;
    }
    if (coups_valide == 1)
    {

        envoyer_plateau(jeu);
        usleep(2000);
        if (jeu->current == jeu->joueur1)
        {
            jeu->current = jeu->joueur2;
        }
        else
        {
            jeu->current = jeu->joueur1;
        }
        envoyer_le_joueur_courant(jeu);
        usleep(2000);
        demander_case_depart(jeu->current);
    }
}

void commande_modifierBio(Joueur *joueur, char body[MAX_BODY_SIZE])
{
    strcpy(joueur->bio, body);
    envoyer_bio(joueur);
}

void commande_consulterBio(Joueur *joueur, Lobby *lobby, char body[MAX_BODY_SIZE])
{
    Joueur *j = trouver_joueur(lobby, body);
    if (j == NULL)
    {
        envoyer_erreur(joueur, "Le joueur n'existe pas ou n'est pas connecté");
    }
    else
    {
        envoyer_consulter_bio(joueur, j);
    }
}
void commande_classement(Joueur *joueur, Lobby *lobby)
{
    envoyer_classement(lobby, joueur);
}
