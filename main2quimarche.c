#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "graphe.h"
#include "chainealimentaire.h"


void nettoyerLigne(char *ligne) {
    char *pos;
    if ((pos = strchr(ligne, '\n')) != NULL) *pos = '\0'; // Supprime '\n'
    if ((pos = strchr(ligne, '\r')) != NULL) *pos = '\0'; // Supprime '\r'
}

void lireFichier(const char *nomFichier, Sommet sommets[], int *nbSommets, Arc arcs[], int *nbArcs) {
    FILE *fichier = fopen(nomFichier, "r");
    if (!fichier) {
        printf("Erreur : impossible d'ouvrir le fichier '%s'.\n", nomFichier);
        exit(EXIT_FAILURE);
    }

    char ligne[256];

    // Lecture des métadonnées
    for (int i = 0; i < 4; i++) {
        if (fgets(ligne, sizeof(ligne), fichier)) {
            nettoyerLigne(ligne); // Nettoyer la ligne
            char *value = strchr(ligne, ':');
            if (value) {
                value++; // Ignorer le préfixe avant ':'
                while (*value == ' ') value++; // Supprimer les espaces
                switch (i) {
                    case 0:
                        printf("Nom : %s\n", value);
                        break;
                    case 1:
                        printf("Region : %s\n", value);
                        break;
                    case 2:
                        printf("Climat : %s\n", value);
                        break;
                    case 3:
                        printf("Description : %s\n", value);
                        break;
                }
            }
        }
    }

    // Lecture du nombre de sommets
    int totalSommets = 0;
    if (fgets(ligne, sizeof(ligne), fichier)) {
        nettoyerLigne(ligne);
        if (sscanf(ligne, "%d", &totalSommets) != 1) {
            printf("Erreur : Impossible de lire le nombre de sommets.\n");
            fclose(fichier);
            exit(EXIT_FAILURE);
        }
        printf("Nombre de sommets : %d\n", totalSommets);
    }

    // Lire les sommets
    for (int i = 0; i < totalSommets; i++) {
        if (fgets(ligne, sizeof(ligne), fichier)) {
            nettoyerLigne(ligne);
            char id[50];
            char type[50];
            float population;

            if (sscanf(ligne, "%49s %49[^ ] %f", id, type, &population) == 3) {
                strcpy(sommets[*nbSommets].nom, id); // Utilise directement l'identifiant comme chaîne
                strcpy(sommets[*nbSommets].type, type);
                sommets[*nbSommets].population = (int)population;
                (*nbSommets)++;
            } else {
                printf("Erreur : Ligne invalide pour un sommet : %s\n", ligne);
                fclose(fichier);
                exit(EXIT_FAILURE);
            }
        }
    }

    // Lecture du nombre d'arcs
    int totalArcs = 0;
    if (fgets(ligne, sizeof(ligne), fichier)) {
        nettoyerLigne(ligne);
        if (sscanf(ligne, "%d", &totalArcs) != 1) {
            printf("Erreur : Impossible de lire le nombre d'arcs.\n");
            fclose(fichier);
            exit(EXIT_FAILURE);
        }
        printf("Nombre d'arcs : %d\n", totalArcs);
    }

    // Lire les arcs
    for (int i = 0; i < totalArcs; i++) {
        if (fgets(ligne, sizeof(ligne), fichier)) {
            nettoyerLigne(ligne);
            char origine[50], destination[50];
            float poids;

            if (sscanf(ligne, "%49s %49s %f", origine, destination, &poids) == 3) {
                strcpy(arcs[*nbArcs].origine, origine);
                strcpy(arcs[*nbArcs].destination, destination);
                arcs[*nbArcs].poids = poids;
                (*nbArcs)++;
            } else {
                printf("Erreur : Ligne invalide pour un arc : %s\n", ligne);
                fclose(fichier);
                exit(EXIT_FAILURE);
            }
        }
    }

    fclose(fichier);
}


void afficherPredecesseursEtSuccesseurs(Sommet sommets[], int nbSommets, Arc arcs[], int nbArcs) {
    for (int i = 0; i < nbSommets; i++) {
        printf("\nSommet : %s (%s)\n", sommets[i].nom, sommets[i].type);

        // Trouver les successeurs
        printf("  Successeurs : ");
        int successeurTrouve = 0;
        for (int j = 0; j < nbArcs; j++) {
            if (strcmp(arcs[j].origine, sommets[i].nom) == 0) {
                printf("%s ", arcs[j].destination);
                successeurTrouve = 1;
            }
        }
        if (!successeurTrouve) {
            printf("Aucun");
        }

        // Trouver les prédécesseurs
        printf("\n  Prédécesseurs : ");
        int predecesseurTrouve = 0;
        for (int j = 0; j < nbArcs; j++) {
            if (strcmp(arcs[j].destination, sommets[i].nom) == 0) {
                printf("%s ", arcs[j].origine);
                predecesseurTrouve = 1;
            }
        }
        if (!predecesseurTrouve) {
            printf("Aucun");
        }

        printf("\n");
    }
}

void dfsConnexite(int current, bool visited[], Sommet sommets[], int nbSommets, Arc arcs[], int nbArcs) {
    visited[current] = true;
    for (int i = 0; i < nbArcs; i++) {
        // Si l'origine de l'arc correspond au sommet actuel
        if (strcmp(arcs[i].origine, sommets[current].nom) == 0) {
            // Trouver l'indice du sommet destination
            for (int j = 0; j < nbSommets; j++) {
                if (strcmp(arcs[i].destination, sommets[j].nom) == 0 && !visited[j]) {
                    dfsConnexite(j, visited, sommets, nbSommets, arcs, nbArcs);
                }
            }
        }
    }
}

bool verifierConnexite(Sommet sommets[], int nbSommets, Arc arcs[], int nbArcs) {
    bool visited[MAX_SOMMETS] = {false};

    // Lancer DFS à partir du premier sommet
    dfsConnexite(0, visited, sommets, nbSommets, arcs, nbArcs);

    // Vérifier si tous les sommets ont été visités
    for (int i = 0; i < nbSommets; i++) {
        if (!visited[i]) {
            return false; // Le graphe n'est pas connexe
        }
    }

    return true; // Le graphe est connexe
}

void premiermaillon(Sommet sommets[], int nbSommets, Arc arcs[], int nbArcs) {
    printf("\nEspèce sans source d'alimentation :\n");
    for (int i = 0; i < nbSommets; i++) {
        int predecesseurTrouve = 0;
        for (int j = 0; j < nbArcs; j++) {
            if (strcmp(arcs[j].destination, sommets[i].nom) == 0) {
                predecesseurTrouve = 1;
                break;
            }
        }
        if (!predecesseurTrouve) {
            printf("%s (%s)\n", sommets[i].nom, sommets[i].type);
        }
    }
}

void derniermaillon(Sommet sommets[], int nbSommets, Arc arcs[], int nbArcs) {
    printf("\nEspèce sans predateur : \n");
    for (int i = 0; i < nbSommets; i++) {
        int successeurTrouve = 0;
        for (int j = 0; j < nbArcs; j++) {
            if (strcmp(arcs[j].origine, sommets[i].nom) == 0) {
                successeurTrouve = 1;
                break;
            }
        }
        if (!successeurTrouve) {
            printf("%s (%s)\n", sommets[i].nom, sommets[i].type);
        }
    }
}

void unpredateur(Sommet sommets[], int nbSommets, Arc arcs[], int nbArcs) {
    printf("\nEspèce avec un seul prédateur :\n");
    for (int i = 0; i < nbSommets; i++) {
        int nombrePredecesseurs = 0;
        for (int j = 0; j < nbArcs; j++) {
            if (strcmp(arcs[j].destination, sommets[i].nom) == 0) {
                nombrePredecesseurs++;
            }
        }
        if (nombrePredecesseurs == 1) {
            printf("%s (%s)\n", sommets[i].nom, sommets[i].type);
        }
    }
}

void unesource(Sommet sommets[], int nbSommets, Arc arcs[], int nbArcs) {
    printf("\nEspèce avec une seule source d'alimentation :\n");
    for (int i = 0; i < nbSommets; i++) {
        int nombreSuccesseurs = 0;
        for (int j = 0; j < nbArcs; j++) {
            if (strcmp(arcs[j].origine, sommets[i].nom) == 0) {
                nombreSuccesseurs++;
            }
        }
        if (nombreSuccesseurs == 1) {
            printf("%s (%s)\n", sommets[i].nom, sommets[i].type);
        }
    }
}



int main(void) {
    Sommet sommets[MAX_SOMMETS];
    Arc arcs[MAX_ARCS];
    int nbSommets = 0, nbArcs = 0;
    char ligne[200];

    //Choix du fichier
    char nomfichier[20];
    printf("Saisir le nom du réseau trophique :");
    scanf("%s", nomfichier);
    printf("%s\n", nomfichier);

    // Ouverture du fichier d'entrée
    FILE *fichier = fopen(nomfichier, "r");
    if (!fichier) {
        printf("Erreur : impossible d'ouvrir le fichier '%s'.\n", nomfichier);
        return 1;
    }



    lireFichier(nomfichier, sommets, &nbSommets, arcs, &nbArcs);

    // Afficher les sommets
    printf("\n--- Sommets ---\n");
    for (int i = 0; i < nbSommets; i++) {
        printf("Sommet %d : %s (%s), Population : %d\n",
               i + 1, sommets[i].nom, sommets[i].type, sommets[i].population);
    }

    // Afficher les arcs
    printf("\n--- Arcs ---\n");
    for (int i = 0; i < nbArcs; i++) {
        printf("Arc %d : %s -> %s (Poids : %.2f)\n",
               i + 1, arcs[i].origine, arcs[i].destination, arcs[i].poids);
    }

    // Afficher les prédécesseurs et successeurs
    printf("\n--- Prédécesseurs et Successeurs ---\n");
    afficherPredecesseursEtSuccesseurs(sommets, nbSommets, arcs, nbArcs);

    // Vérifier la connexité du graphe
    if (verifierConnexite(sommets, nbSommets, arcs, nbArcs)) {
        printf("\nLe graphe est connexe.\n");
    } else {
        printf("\nLe graphe n'est pas connexe.\n");
    }

    // espece sans predateur
    premiermaillon(sommets, nbSommets, arcs, nbArcs);
    //espece qui n'en consomment pas d'autres
    derniermaillon(sommets, nbSommets, arcs, nbArcs);

// Afficher les espèces avec un seul prédateur
    unpredateur(sommets, nbSommets, arcs, nbArcs);

    // Afficher les espèces avec une seule source de nourriture
    unesource(sommets, nbSommets, arcs, nbArcs);

    // Afficher toutes les chaînes alimentaires
    afficherToutesLesChainesAlimentaires(sommets, nbSommets, arcs, nbArcs);

// Identifier les espèces avec une seule source
    printf("\n--- Especes avec une seule source de nourriture ---\n");
    for (int i = 0; i < nbSommets; i++) {
        if (verifierSourceUnique(sommets[i].nom, sommets, nbSommets, arcs, nbArcs)) {
            printf("Espece avec une seule source de nourriture : %s (%s)\n", sommets[i].nom, sommets[i].type);
        }
    }


    return 0;
}
