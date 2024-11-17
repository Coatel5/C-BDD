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
    nouveau->gauche = NULL;
    nouveau->droite = NULL;
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
    fprintf(fichier, "%d|%s|%d|%d\n",
            noeud->id,
            noeud->contenu,
            noeud->gauche ? noeud->gauche->id : -1,
            noeud->droite ? noeud->droite->id : -1);
}

ArbreNoeud* chargerNoeudTexte(char* ligne) {
    ArbreNoeud* noeud = (ArbreNoeud*)malloc(sizeof(ArbreNoeud));
    if (!noeud) {
        fprintf(stderr, "Erreur : Allocation mémoire échouée.\n");
        exit(EXIT_FAILURE);
    }
    int gauche, droite;
    sscanf(ligne, "%d|%99[^|]|%d|%d",
           &noeud->id,
           noeud->contenu,
           &gauche,
           &droite);
    noeud->gauche = (gauche != -1) ? (ArbreNoeud*)(long)gauche : NULL;
    noeud->droite = (droite != -1) ? (ArbreNoeud*)(long)droite : NULL;
    return noeud;
}

ArbreNoeud* chargerArbreTexte(FILE* fichier) {
    char ligne[256];
    ArbreNoeud* noeuds[1000] = {NULL}; 
    ArbreNoeud* racine = NULL;

    while (fgets(ligne, sizeof(ligne), fichier)) {
        ArbreNoeud* noeud = (ArbreNoeud*)malloc(sizeof(ArbreNoeud));
        if (!noeud) {
            fprintf(stderr, "Erreur : Allocation mémoire échouée.\n");
            exit(EXIT_FAILURE);
        }
        int gauche, droite;
        sscanf(ligne, "%d|%99[^|]|%d|%d",
               &noeud->id,
               noeud->contenu,
               &gauche,
               &droite);

        noeud->gauche = (gauche != -1) ? (ArbreNoeud*)(long)gauche : NULL;
        noeud->droite = (droite != -1) ? (ArbreNoeud*)(long)droite : NULL;

        noeuds[noeud->id] = noeud;

        if (racine == NULL) {
            racine = noeud;
        }
    }

    for (int i = 0; i < 1000; i++) {
        if (noeuds[i] != NULL) {
            ArbreNoeud* noeud = noeuds[i];
            if (noeud->gauche != NULL) {
                noeud->gauche = noeuds[(long)noeud->gauche];
            }
            if (noeud->droite != NULL) {
                noeud->droite = noeuds[(long)noeud->droite];
            }
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
    if (racine == NULL) {
        fprintf(stderr, "Erreur : Arbre vide.\n");
        return -1;
    }

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

    if (idCourant != NULL) {
        (*idCourant)++;
    }
    ArbreNoeud* nouveau = creerNoeud(*idCourant, contenu);

    if (direction == 'G') {
        parent->gauche = nouveau;
    } else if (direction == 'D') {
        parent->droite = nouveau;
    }

    return nouveau->id;
}
