#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Estrutura: Sala (árvore binária)
typedef struct Sala {
    char *nome;
    struct Sala *esq;
    struct Sala *dir;
} Sala;

Sala* criarSala(const char *nome) {
    Sala *s = (Sala*) malloc(sizeof(Sala));
    s->nome = strdup(nome);
    s->esq = s->dir = NULL;
    return s;
}

// Estrutura: Pista
typedef struct Pista {
    char *texto;
    struct Pista *esq;
    struct Pista *dir;
} Pista;