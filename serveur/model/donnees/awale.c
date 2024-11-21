#include "awale.h"

void initialiserPlateau(Jeu *jeu)
{
    if (jeu->plateau == NULL)
    {
        Case *plateau = malloc(sizeof(Case) * 12);
        jeu->plateau = plateau;
    }

    // Initialiser les graines et les propriétaires des cases
    for (int i = 0; i < 12; i++)
    {
        jeu->plateau[i].nbGraines = 4; // 4 graines par case
    }
    for (int i = 0; i < 6; i++)
    {
        jeu->plateau[i].proprietaire = jeu->joueur1; // Cases 0-5 appartiennent à joueur1
    }
    for (int i = 6; i < 12; i++)
    {
        jeu->plateau[i].proprietaire = jeu->joueur2; // Cases 6-11 appartiennent à joueur2
    }

    // Initialiser les scores des joueurs
    jeu->joueur1->score = 0;
    jeu->joueur2->score = 0;
}
int verifierCasDarret(Jeu *jeu)
{

    // Si un joueur a un score de 25 ou plus, il gagne

    if (jeu->joueur1->score >= 25)
    {
        jeu->vainqueur = jeu->joueur1;
    }
    else if (jeu->joueur2->score >= 25)
    {
        jeu->vainqueur = jeu->joueur2;
    }

    // L'égalité return 0
    if (jeu->joueur1->score == 24 && jeu->joueur2->score == 24)
    {
        return 0;
    }

    return 1;
}
void calculerScore(Jeu *jeu, int caseFin)
{
    // si on a 2 ou 3 graines dans une case on les prend
    // 4 6 3 11
    while ((jeu->plateau[caseFin].nbGraines == 2 || jeu->plateau[caseFin].nbGraines == 3) && jeu->plateau[caseFin].proprietaire != jeu->current)
    {
        jeu->current->score += jeu->plateau[caseFin].nbGraines;
        jeu->plateau[caseFin].nbGraines = 0;
        caseFin--;
    }
}
int jouerCoup(Jeu *jeu, int caseDepart)
{

    if (jeu->plateau[caseDepart].proprietaire == jeu->current)
    {
        int nbGraines = jeu->plateau[caseDepart].nbGraines;
        jeu->plateau[caseDepart].nbGraines = 0;
        int i = caseDepart + 1;
        while (nbGraines > 0)
        {
            if (i == 12)
            {
                i = 0;
            }
            if (i != caseDepart)
            {
                jeu->plateau[i].nbGraines++;
                nbGraines--;
            }
            i++;
        }
        calculerScore(jeu, i - 1);
    }
    else
    {
        return -1;
    }

    return verifierCasDarret(jeu);
}
