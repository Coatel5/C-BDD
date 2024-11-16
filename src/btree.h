#ifndef BTREE_H
#define BTREE_H

#include <stdio.h>

typedef struct ArbreNoeud {
    int id;                 // idunique
    char contenu[100];      
    long offsetDroite;      
    long offsetGauche;      
} ArbreNoeud;

// Fonctions principales
ArbreNoeud* creerNoeud(int id, const char* contenu);
long sauvegarderNoeud(FILE* fichier, ArbreNoeud* noeud);
void mettreAJourNoeud(FILE* fichier, long offset, ArbreNoeud* noeud);
ArbreNoeud* chargerNoeud(FILE* fichier, long offset);
long ajouterDonnee(FILE* fichier, int parentID, const char* contenu, char direction, int* idCourant);
void afficherArbre(FILE* fichier, long offset, int profondeur);
long rechercherOffsetParID(FILE* fichier, int idRecherche);

#endif
