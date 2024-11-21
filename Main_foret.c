#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "foret.h" // Inclusion du header

int main() {
    Sommet sommets[MAX_SOMMETS];
    Arc arcs[MAX_ARCS];
    int nbSommets = 0, nbArcs = 0;
    char ligne[200];

    // Ouverture du fichier d'entrée
    FILE *fichier = fopen(FICHIER_ENTREE, "r");
    if (!fichier) {
        printf("Erreur : impossible d'ouvrir le fichier '%s'.\n", FICHIER_ENTREE);
        return 1;
    }

    // Lecture du fichier ligne par ligne
    while (fgets(ligne, sizeof(ligne), fichier)) {
        // Identifier les sections
        if (strncmp(ligne, "# Sommets", 9) == 0) {
            while (fgets(ligne, sizeof(ligne), fichier) && ligne[0] != '#') {
                char nom[50], type[50];
                int population;
                if (sscanf(ligne, "%[^:]: %[^,], %d", nom, type, &population) == 3) {
                    strcpy(sommets[nbSommets].nom, nom);
                    strcpy(sommets[nbSommets].type, type);
                    sommets[nbSommets].population = population;
                    nbSommets++;
                } else if (sscanf(ligne, "%[^:]: %[^,], -", nom, type) == 2) {
                    strcpy(sommets[nbSommets].nom, nom);
                    strcpy(sommets[nbSommets].type, type);
                    sommets[nbSommets].population = -1;
                    nbSommets++;
                }
            }
        } else if (strncmp(ligne, "# Arcs", 6) == 0) {
            while (fgets(ligne, sizeof(ligne), fichier) && ligne[0] != '#') {
                char origine[50], destination[50];
                float poids;
                if (sscanf(ligne, "%[^->]->%[^:]: %f", origine, destination, &poids) == 3) {
                    strcpy(arcs[nbArcs].origine, origine);
                    strcpy(arcs[nbArcs].destination, destination);
                    arcs[nbArcs].poids = poids;
                    nbArcs++;
                }
            }
        }
    }
    fclose(fichier);

    // Création des fichiers pour Gephi
    FILE *nodesFile = fopen(FICHIER_SOMMETS, "w");
    if (!nodesFile) {
        printf("Erreur : impossible de créer le fichier '%s'.\n", FICHIER_SOMMETS);
        return 1;
    }
    fprintf(nodesFile, "Id,Label,Type,Population\n");
    for (int i = 0; i < nbSommets; i++) {
        fprintf(nodesFile, "%d,%s,%s,%d\n", i + 1, sommets[i].nom, sommets[i].type, sommets[i].population);
    }
    fclose(nodesFile);

    FILE *edgesFile = fopen(FICHIER_ARCS, "w");
    if (!edgesFile) {
        printf("Erreur : impossible de créer le fichier '%s'.\n", FICHIER_ARCS);
        return 1;
    }
    fprintf(edgesFile, "Source,Target,Weight\n");
    for (int i = 0; i < nbArcs; i++) {
        int source = -1, target = -1;
        for (int j = 0; j < nbSommets; j++) {
            if (strcmp(arcs[i].origine, sommets[j].nom) == 0) source = j + 1;
            if (strcmp(arcs[i].destination, sommets[j].nom) == 0) target = j + 1;
        }
        if (source != -1 && target != -1) {
            fprintf(edgesFile, "%d,%d,%.2f\n", source, target, arcs[i].poids);
        }
    }
    fclose(edgesFile);

    printf("Fichiers '%s' et '%s' générés avec succès.\n", FICHIER_SOMMETS, FICHIER_ARCS);
    return 0;
}
