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

char *afficherPlateau(Jeu *jeu)
{

    // Taille fixe pour éviter les erreurs de dépassement
    char buffer[5000];
    int pos = 0; // Position actuelle dans le buffer

    // Informations sur le joueur 1
    pos += snprintf(buffer + pos, sizeof(buffer) - pos, "Joueur 1 : %s\nScore : %d\n\n",
                    jeu->joueur1->nom, jeu->joueur1->score);

    // Informations sur le joueur 2
    pos += snprintf(buffer + pos, sizeof(buffer) - pos, "Joueur 2 : %s\nScore : %d\n\n",
                    jeu->joueur2->nom, jeu->joueur2->score);

    // Rangée supérieure : indices
    for (int i = 0; i < 6; i++)
    {
        pos += snprintf(buffer + pos, sizeof(buffer) - pos, "  %d  ", i);
    }
    pos += snprintf(buffer + pos, sizeof(buffer) - pos, "\n");

    // Rangée supérieure : graines
    for (int i = 0; i < 6; i++)
    {
        pos += snprintf(buffer + pos, sizeof(buffer) - pos, "  %d  ", jeu->plateau[i].nbGraines);
    }
    pos += snprintf(buffer + pos, sizeof(buffer) - pos, "\n");

    // Ligne de séparation
    pos += snprintf(buffer + pos, sizeof(buffer) - pos, "-------------------------------\n");

    // Rangée inférieure : indices
    for (int i = 11; i > 5; i--)
    {
        pos += snprintf(buffer + pos, sizeof(buffer) - pos, "  %d  ", i);
    }
    pos += snprintf(buffer + pos, sizeof(buffer) - pos, "\n");

    // Rangée inférieure : graines
    for (int i = 11; i > 5; i--)
    {
        pos += snprintf(buffer + pos, sizeof(buffer) - pos, "  %d  ", jeu->plateau[i].nbGraines);
    }
    pos += snprintf(buffer + pos, sizeof(buffer) - pos, "\n");

    // Allouer dynamiquement la chaîne finale
    char *result = strdup(buffer);
    if (result == NULL)
    {
        perror("Erreur d'allocation mémoire pour le plateau");
        exit(EXIT_FAILURE);
    }

    return result;
}

int verifierCasDarret(Jeu *jeu)
{
    // Si un joueur a un score de 25 ou plus, il gagne

    if (jeu->joueur1->score >= 2)
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

/*int main(){
    Plateau plateau;
    Joueur joueur1;
    joueur1.nom="Joueur 1";
    Joueur joueur2;
    joueur2.nom="Joueur 2";
    plateau.joueur1=&joueur1;
    plateau.joueur2=&joueur2;
    initialiserPlateau(&plateau);
    afficherPlateau(&plateau);
    Joueur* vainqueur=NULL;
    while(vainqueur==NULL){
        int caseDepart;
        do{
            printf("Joueur 1, entrez la case de départ : ");
            scanf("%d",&caseDepart);
            vainqueur=jouerCoup(&plateau,caseDepart, &joueur1);
        } while(vainqueur==-1);
        afficherPlateau(&plateau);
        if(vainqueur!=NULL){
            break;
        }
        printf("____________________________________________________\n \n");
        do{
            printf("Joueur 2, entrez la case de départ : ");
            scanf("%d",&caseDepart);
            vainqueur=jouerCoup(&plateau,caseDepart, &joueur2);
        } while(vainqueur==-1 );
        afficherPlateau(&plateau);
    }
    afficherFin(&plateau,vainqueur);
    return 0;
}*/