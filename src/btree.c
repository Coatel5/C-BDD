#include "btree.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

ArbreNoeud* creerNoeud(int id, const char* contenu) {
    ArbreNoeud* nouveau = (ArbreNoeud*)malloc(sizeof(ArbreNoeud));
    if (!nouveau) {
        fprintf(stderr, "Erreur : Allocation mémoire échouée.\n");
        exit(EXIT_FAILURE);
    }
    nouveau->id = id;
    strncpy(nouveau->contenu, contenu, sizeof(nouveau->contenu) - 1);
    nouveau->contenu[sizeof(nouveau->contenu) - 1] = '\0';
    nouveau->offsetDroite = -1;
    nouveau->offsetGauche = -1;
    return nouveau;
}

long sauvegarderNoeud(FILE* fichier, ArbreNoeud* noeud) {
    fseek(fichier, 0, SEEK_END); // Aller à la fin du fichier
    long offset = ftell(fichier); // Obtenir la position actuelle
    fwrite(noeud, sizeof(ArbreNoeud), 1, fichier); // Écrire le nœud
    return offset; // Retourner l'offset du nœud
}

void mettreAJourNoeud(FILE* fichier, long offset, ArbreNoeud* noeud) {
    fseek(fichier, offset, SEEK_SET);
    fwrite(noeud, sizeof(ArbreNoeud), 1, fichier);
}

ArbreNoeud* chargerNoeud(FILE* fichier, long offset) {
    fseek(fichier, offset, SEEK_SET); // Aller à l'offset spécifié
    ArbreNoeud* noeud = (ArbreNoeud*)malloc(sizeof(ArbreNoeud));
    if (!noeud) {
        fprintf(stderr, "Erreur : Allocation mémoire échouée.\n");
        exit(EXIT_FAILURE);
    }
    fread(noeud, sizeof(ArbreNoeud), 1, fichier); // Lire le nœud
    return noeud;
}

long rechercherOffsetParID(FILE* fichier, int idRecherche) {
    fseek(fichier, 0, SEEK_SET); 

    while (!feof(fichier)) {
        long offset = ftell(fichier);
        ArbreNoeud* noeud = chargerNoeud(fichier, offset);

        if (noeud->id == idRecherche) {
            free(noeud);
            return offset; 
        }

        free(noeud);
    }

    return -1; 
}

long ajouterDonnee(FILE* fichier, int parentID, const char* contenu, char direction, int* idCourant) {
    long parentOffset = rechercherOffsetParID(fichier, parentID);
    if (parentOffset == -1) {
        fprintf(stderr, "Erreur : Parent avec ID %d introuvable.\n", parentID);
        return -1;
    }

    ArbreNoeud* parent = chargerNoeud(fichier, parentOffset);

    if (direction == 'G' && parent->offsetGauche != -1) {
        fprintf(stderr, "Erreur : Le côté gauche de l'ID %d est déjà occupé.\n", parent->id);
        free(parent);
        return -1;
    }
    if (direction == 'D' && parent->offsetDroite != -1) {
        fprintf(stderr, "Erreur : Le côté droit de l'ID %d est déjà occupé.\n", parent->id);
        free(parent);
        return -1;
    }

    (*idCourant)++;
    ArbreNoeud* nouveau = creerNoeud(*idCourant, contenu);
    long offsetNouveau = sauvegarderNoeud(fichier, nouveau);

    if (direction == 'G') {
        parent->offsetGauche = offsetNouveau;
    } else if (direction == 'D') {
        parent->offsetDroite = offsetNouveau;
    }
    mettreAJourNoeud(fichier, parentOffset, parent);

    free(parent);
    free(nouveau);
    return offsetNouveau;
}

void afficherArbre(FILE* fichier, long offset, int profondeur) {
    if (offset == -1) {
        return;
    }

    ArbreNoeud* noeud = chargerNoeud(fichier, offset);

    afficherArbre(fichier, noeud->offsetDroite, profondeur + 1);

    for (int i = 0; i < profondeur; i++) {
        printf("    ");
    }
    printf("%d (%s)\n", noeud->id, noeud->contenu);

    afficherArbre(fichier, noeud->offsetGauche, profondeur + 1);

    free(noeud);
}
