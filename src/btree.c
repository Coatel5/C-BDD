#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "btree.h"

ArbreNoeud* creerNoeud(int id, const char* contenu) {
    ArbreNoeud* nouveau = (ArbreNoeud*)malloc(sizeof(ArbreNoeud));
    if (!nouveau) {
        fprintf(stderr, "Erreur : Allocation mémoire échouée.\n");
        exit(EXIT_FAILURE);
    }
    nouveau->id = id;
    strncpy(nouveau->contenu, contenu, sizeof(nouveau->contenu) - 1);
    nouveau->contenu[sizeof(nouveau->contenu) - 1] = '\0';
    nouveau->offsetGauche = -1;
    nouveau->offsetDroite = -1;
    return nouveau;
}

void libererArbre(ArbreNoeud* racine) {
    if (racine != NULL) {
        libererArbre(racine->gauche);
        libererArbre(racine->droite);
        free(racine);
    }
}

void sauvegarderNoeudTexte(FILE* fichier, ArbreNoeud* noeud) {
    fprintf(fichier, "%d|%s|%ld|%ld\n", 
            noeud->id, 
            noeud->contenu, 
            noeud->offsetGauche, 
            noeud->offsetDroite);
}

ArbreNoeud* chargerNoeudTexte(char* ligne) {
    ArbreNoeud* noeud = (ArbreNoeud*)malloc(sizeof(ArbreNoeud));
    if (!noeud) {
        fprintf(stderr, "Erreur : Allocation mémoire échouée.\n");
        exit(EXIT_FAILURE);
    }
    sscanf(ligne, "%d|%99[^|]|%ld|%ld", 
           &noeud->id, 
           noeud->contenu, 
           &noeud->offsetGauche, 
           &noeud->offsetDroite);
    return noeud;
}

ArbreNoeud* chargerArbreTexte(FILE* fichier) {
    char ligne[256];
    ArbreNoeud* racine = NULL;
    ArbreNoeud* noeuds[1000] = {NULL};

    while (fgets(ligne, sizeof(ligne), fichier)) {
        ArbreNoeud* noeud = chargerNoeudTexte(ligne);
        noeuds[noeud->id] = noeud;

        if (racine == NULL) {
            racine = noeud;
        }

        if (noeud->offsetGauche != -1) {
            noeud->gauche = noeuds[noeud->offsetGauche];
        }
        if (noeud->offsetDroite != -1) {
            noeud->droite = noeuds[noeud->offsetDroite];
        }
    }

    return racine;
}

void sauvegarderArbreTexte(FILE* fichier, ArbreNoeud* racine) {
    if (racine == NULL) {
        return;
    }

    sauvegarderNoeudTexte(fichier, racine);
    sauvegarderArbreTexte(fichier, racine->gauche);
    sauvegarderArbreTexte(fichier, racine->droite);
}

void afficherArbreTexte(ArbreNoeud* racine, int profondeur) {
    if (racine == NULL) {
        return;
    }

    afficherArbreTexte(racine->droite, profondeur + 1);

    for (int i = 0; i < profondeur; i++) {
        printf("    ");
    }
    printf("%d (%s)\n", racine->id, racine->contenu);

    afficherArbreTexte(racine->gauche, profondeur + 1);
}

long ajouterDonneeTexte(ArbreNoeud* racine, int parentID, const char* contenu, char direction, int* idCourant) {
    ArbreNoeud* parent = NULL;
    ArbreNoeud* stack[1000]; 
    int top = 0;

    stack[top++] = racine;
    while (top > 0) {
        ArbreNoeud* courant = stack[--top];
        if (courant->id == parentID) {
            parent = courant;
            break;
        }
        if (courant->droite) {
            stack[top++] = courant->droite;
        }
        if (courant->gauche) {
            stack[top++] = courant->gauche;
        }
    }

    if (parent == NULL) {
        fprintf(stderr, "Erreur : Parent avec ID %d introuvable.\n", parentID);
        return -1;
    }

    if (direction == 'G' && parent->gauche != NULL) {
        fprintf(stderr, "Erreur : Le côté gauche de l'ID %d est déjà occupé.\n", parent->id);
        return -1;
    }
    if (direction == 'D' && parent->droite != NULL) {
        fprintf(stderr, "Erreur : Le côté droit de l'ID %d est déjà occupé.\n", parent->id);
        return -1;
    }

    (*idCourant)++;
    ArbreNoeud* nouveau = creerNoeud(*idCourant, contenu);

    if (direction == 'G') {
        parent->gauche = nouveau;
        parent->offsetGauche = nouveau->id;
    } else if (direction == 'D') {
        parent->droite = nouveau;
        parent->offsetDroite = nouveau->id;
    }

    return nouveau->id;
}
