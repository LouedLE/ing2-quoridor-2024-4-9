#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graphe.h"


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

    return 0;
}

