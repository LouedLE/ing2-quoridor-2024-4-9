
#ifndef TEST_GRAPHE_H
#define TEST_GRAPHE_H

// Définition de constantes
#define MAX_SOMMETS 100
#define MAX_ARCS 200
#define FICHIER_SOMMETS "nodes.csv"
#define FICHIER_ARCS "edges.csv"

// Déclarations des structures
typedef struct {
    char nom[50];
    char type[50];
    int population; // -1 si non applicable
} Sommet;

typedef struct {
    char origine[50];
    char destination[50];
    float poids; // Pondération
} Arc;

#endif //TEST_GRAPHE_H
