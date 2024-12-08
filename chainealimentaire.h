//
// Created by mathu on 08/12/2024.
//

#ifndef TEST2_CHAINEALIMENTAIRE_H
#define TEST2_CHAINEALIMENTAIRE_H

#include "graphe.h"
#include "stdio.h"
#include "string.h"

void remonterChaineAlimentaire(
        const char *espece,
        Sommet sommets[],
        int nbSommets,
        Arc arcs[],
        int nbArcs,
        char chemin[][50],
        int index,
        bool afficher
) {
    bool hasPredecesseur = false;

    for (int i = 0; i < nbArcs; i++) {
        if (strcmp(arcs[i].destination, espece) == 0) {
            hasPredecesseur = true;

            // Ajouter le prédecesseur au chemin
            strcpy(chemin[index], arcs[i].origine);

            // Appeler récursivement pour le prédecesseur
            remonterChaineAlimentaire(
                    arcs[i].origine,
                    sommets,
                    nbSommets,
                    arcs,
                    nbArcs,
                    chemin,
                    index + 1,
                    afficher
            );
        }
    }

    // Si aucun prédecesseur, afficher la chaîne alimentaire complète
    if (!hasPredecesseur && afficher) {
        printf("Chaîne alimentaire : ");
        for (int i = index - 1; i >= 0; i--) { // Remonter le chemin
            printf("%s ", chemin[i]);
            if (i > 0) printf("-> ");
        }
        printf("\n");
    }
}

void afficherChainesAlimentaires(
        const char *espece,
        Sommet sommets[],
        int nbSommets,
        Arc arcs[],
        int nbArcs
) {
    printf("\n--- Chaînes alimentaires pour l'espèce : %s ---\n", espece);

    char chemin[100][50]; // Stocker une chaîne alimentaire
    strcpy(chemin[0], espece);

    remonterChaineAlimentaire(espece, sommets, nbSommets, arcs, nbArcs, chemin, 1, true);
}

void afficherToutesLesChainesAlimentaires(Sommet sommets[], int nbSommets, Arc arcs[], int nbArcs) {
    printf("\n--- Toutes les chaînes alimentaires ---\n");
    for (int i = 0; i < nbSommets; i++) {
        printf("\nEspèce cible : %s\n", sommets[i].nom);
        afficherChainesAlimentaires(sommets[i].nom, sommets, nbSommets, arcs, nbArcs);
    }
}


bool verifierSourceUnique(const char *espece, Sommet sommets[], int nbSommets, Arc arcs[], int nbArcs) {
    char chemin[100][50];
    char sources[100][50];
    int nbSources = 0;

    remonterChaineAlimentaire(espece, sommets, nbSommets, arcs, nbArcs, chemin, 1, false);

    for (int i = 0; i < nbArcs; i++) {
        if (strcmp(arcs[i].destination, espece) == 0) {
            strcpy(chemin[0], arcs[i].origine);
            remonterChaineAlimentaire(arcs[i].origine, sommets, nbSommets, arcs, nbArcs, chemin, 1, false);

            // Ajouter la source si elle n'existe pas encore
            bool sourceDejaPresente = false;
            for (int j = 0; j < nbSources; j++) {
                if (strcmp(sources[j], chemin[nbSources - 1]) == 0) {
                    sourceDejaPresente = true;
                    break;
                }
            }
            if (!sourceDejaPresente) {
                strcpy(sources[nbSources++], chemin[nbSources - 1]);
            }
        }
    }

    // Si une seule source, retourne vrai
    return nbSources == 1;
}


#endif //TEST2_CHAINEALIMENTAIRE_H
