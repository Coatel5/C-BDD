#ifndef BTREE_H
#define BTREE_H

#include <stdio.h>

typedef struct ArbreNoeud {
    int id;                
    char contenu[100];      
    long offsetGauche;     
    long offsetDroite;      
    struct ArbreNoeud* gauche;
    struct ArbreNoeud* droite;
} ArbreNoeud;

ArbreNoeud* creerNoeud(int id, const char* contenu);
void libererArbre(ArbreNoeud* racine);

ArbreNoeud* chargerArbreTexte(FILE* fichier);
void sauvegarderArbreTexte(FILE* fichier, ArbreNoeud* racine);

long ajouterDonneeTexte(ArbreNoeud* racine, int parentID, const char* contenu, char direction, int* idCourant);
void afficherArbreTexte(ArbreNoeud* racine, int profondeur);

#endif


