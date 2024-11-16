/** 
#include <stdio.h>
#include <stdlib.h>
#include "repl.c"
#include <stdbool.h>
#include <string.h>


int main(int argc, char* argv[], char* envp[]){
  
  repl();
  return 0;
  
}
*/

#include "btree.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE* fichier = fopen("arbre.bin", "wb+");
    if (!fichier) {
        fprintf(stderr, "Erreur : Impossible d'ouvrir le fichier.\n");
        return 1;
    }

    int idCourant = 1; 

    ArbreNoeud* racine = creerNoeud(idCourant, "root");
    long racineOffset = sauvegarderNoeud(fichier, racine);
    free(racine);

    // Ajouter des données 
    ajouterDonnee(fichier, 1, "droite de root", 'D', &idCourant); // ID = 2
    ajouterDonnee(fichier, 1, "gauche de root", 'G', &idCourant); // ID = 3
    ajouterDonnee(fichier, 2, "droite de 2", 'D', &idCourant);    // ID = 4
    ajouterDonnee(fichier, 2, "gauche de 2", 'G', &idCourant);    // ID = 5
    ajouterDonnee(fichier, 4, "droite de 4", 'D', &idCourant);    // ID = 6
    ajouterDonnee(fichier, 6, "droite de 6", 'D', &idCourant);    // ID = 7
    ajouterDonnee(fichier, 7, "gauche de 7", 'G', &idCourant);    // ID = 8
    ajouterDonnee(fichier, 7, "droite de 7", 'D', &idCourant);    // ID = 9



    // Afficher l'arbre
    printf("\nArbre binaire :\n");
    afficherArbre(fichier, racineOffset, 0);

    fclose(fichier);
    return 0;
}


