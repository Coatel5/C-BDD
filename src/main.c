#include <stdio.h>
#include <stdlib.h>
#include "repl.h"
#include "btree.h"

int main() {
    FILE* fichier = fopen("btree.txt", "r");
    ArbreNoeud* racine = NULL;
    int idCourant = 1;

    if (fichier) {
        printf("Chargement de la base de données depuis btree.txt...\n");
        racine = chargerArbreTexte(fichier);
        fclose(fichier);

        ArbreNoeud* stack[1000];
        int top = 0;
        stack[top++] = racine;
        while (top > 0) {
            ArbreNoeud* noeud = stack[--top];
            if (noeud->id > idCourant) {
                idCourant = noeud->id;
            }
            if (noeud->droite) stack[top++] = noeud->droite;
            if (noeud->gauche) stack[top++] = noeud->gauche;
        }
    } else {
        printf("Aucun fichier trouvé. Création d'un nouvel arbre.\n");
        racine = creerNoeud(idCourant, "root");
    }

    repl(racine, &idCourant);

    fichier = fopen("btree.txt", "w");
    if (fichier) {
        printf("Sauvegarde de la base de données dans btree.txt...\n");
        sauvegarderArbreTexte(fichier, racine);
        fclose(fichier);
    } else {
        fprintf(stderr, "Erreur : Impossible de sauvegarder dans btree.txt.\n");
    }

    libererArbre(racine);
    return 0;
}
