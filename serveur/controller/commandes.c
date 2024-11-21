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
    usleep(2000);

    // Initialiser la partie
    initialiser_jeu(lobby, demandeur, joueur);

    Jeu *jeu = lobby->jeux[joueur->idPartie];
    if (jeu == NULL)
    {
        perror("Erreur d'allocation mémoire pour le jeu");
        exit(1);
    }

    // récupération du plateau de jeu
    // char *string_plateau = afficherPlateau(jeu);
    envoyer_plateau(jeu);
    usleep(2000);

    envoyer_le_joueur_courant(jeu);
    usleep(2000);

    demander_case_depart(jeu->current);
}

void commande_jouerCoup(Lobby *lobby, Joueur *joueur, char body[MAX_BODY_SIZE])
{
    printf("DEBUT COMMANDE JOUER COUP");
    printf("BODY : %s", body);
    printf("case : %d", atoi(body));
    int case_depart = atoi(body);
    printf("CASE DEPART : %d", case_depart);
    Jeu *jeu = lobby->jeux[joueur->idPartie];
    printf("ID JEU %d", joueur->idPartie);
    if (joueur->idPartie == -1)
    {
        printf("erreur id partie");
        envoyer_erreur(joueur);
        return;
    }
    if (joueur->status != PARTIE)
    {
        printf("erreur not in partie");
        envoyer_erreur(joueur);
        return;
    }
    if (jeu->current->nom != joueur->nom)
    {
        printf("Ce n'est pas a ce joueur de jouer");
        envoyer_erreur(joueur);
        return;
    }
    printf("TEST JOUER COUP");
    int coups_valide = jouerCoup(jeu, case_depart);
    printf("COUP : %d", coups_valide);
    printf("TEST COUP INVALIDE");
    if (coups_valide == -1)
    {
        // Le coups est invalide, on redemande a l'utilisateur
        envoyer_erreur(joueur);
        usleep(2000);

        demander_case_depart(joueur);
        return;
    }
    printf("TEST EGALITE");
    if (coups_valide == 0)
    {
        // égalité
        envoyer_egalite(jeu);
        usleep(2000);
        calculerElo(jeu->joueur1, jeu->joueur2, 0.5);
        fin_partie(jeu);
        return;
    }

    printf("TEST VAINQUEUR");
    if (jeu->vainqueur != NULL && (jeu->vainqueur->nom == jeu->joueur1->nom || jeu->vainqueur->nom == jeu->joueur2->nom))
    {
        // On a un vainqueur
        printf("ON A UN VAINQUEUR");
        printf("%s", jeu->vainqueur->nom);
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
    printf("TEST CONTINUER");
    if (coups_valide == 1)
    {
        // on continue la partie :
        // char *string_plateau = malloc(sizeof(char) * 2048);
        // string_plateau = afficherPlateau(jeu);
        printf("AFFICHAGE PLATEAU ET ENVOIE");
        envoyer_plateau(jeu);
        usleep(2000);
        printf("CHANGEMENT JOUEUR");
        if (jeu->current == jeu->joueur1)
        {
            jeu->current = jeu->joueur2;
        }
        else
        {
            jeu->current = jeu->joueur1;
        }
        printf("ENVOIE JOUEUR COURANT");
        envoyer_le_joueur_courant(jeu);
        usleep(2000);
        printf("DEMANDER CASE DEPART");
        demander_case_depart(jeu->current);
    }
    printf("FIN DU COUP %p", jeu->vainqueur);
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
        envoyer_erreur(joueur);
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
