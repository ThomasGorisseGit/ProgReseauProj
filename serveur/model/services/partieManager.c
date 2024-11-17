#include "partieManager.h"

void gerer_coup(Jeu *jeu, Joueur *joueur, int case_depart)
{
    if (joueur->idPartie == -1)
    {
        envoyer_erreur(joueur);
        return;
    }
    if (joueur->status != PARTIE)
    {
        envoyer_erreur(joueur);
        return;
    }
    if (jeu->current->nom != joueur->nom)
    {
        envoyer_erreur(joueur);
        return;
    }
    int coups_valide = jouerCoup(jeu, case_depart);
    if (coups_valide == -1)
    {
        // Le coups est invalide, on redemande a l'utilisateur
        envoyer_erreur(joueur);
        demander_case_depart(joueur);
        return;
    }
    if (coups_valide == 0)
    {
        // égalité
        envoyer_egalite(jeu);
        fin_partie(jeu);
    }
    if (jeu->vainqueur != NULL)
    {
        // On a un vainqueur
        envoyer_gagnant(jeu);
        fin_partie(jeu);
    }

    if (coups_valide == 1)
    {
        // on continue la partie :
        envoyer_plateau(jeu);
        if (jeu->current == jeu->joueur1)
        {
            jeu->current = jeu->joueur2;
        }
        else
        {
            jeu->current = jeu->joueur1;
        }
        envoyer_le_joueur_courant(jeu);
        demander_case_depart_vue(jeu->current);
    }
}