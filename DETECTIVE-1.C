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

Pista* criarPistaNode(const char *texto) {
    Pista *p = (Pista*) malloc(sizeof(Pista));
    p->texto = strdup(texto);
    p->esq = p->dir = NULL;
    return p;
}

Pista* inserirPistaBST(Pista *root, const char *texto) {
    if (!root) return criarPistaNode(texto);
    int cmp = strcmp(texto, root->texto);
    if (cmp < 0) root->esq = inserirPistaBST(root->esq, texto);
    else if (cmp > 0) root->dir = inserirPistaBST(root->dir, texto);
    // se igual, não insere duplicata
    return root;
}

void listarPistasEmOrdem(Pista *root) {
    if (!root) return;
    listarPistasEmOrdem(root->esq);
    printf(" - %s\n", root->texto);
    listarPistasEmOrdem(root->dir);
}

void liberarPistas(Pista *root) {
    if (!root) return;
    liberarPistas(root->esq);
    liberarPistas(root->dir);
    free(root->texto);
    free(root);
}

// Estrutura: Suspeito + tabela hash
typedef struct SuspeitoPista {
    char *textoPista;
    struct SuspeitoPista *prox;
} SuspeitoPista;

typedef struct Suspeito {
    char *nome;
    SuspeitoPista *pistas;
    int contador; // quantas pistas associadas (para probabilidade)
    struct Suspeito *prox; // para lista encadeada da tabela (colisões)
} Suspeito;

#define HASH_SIZE 13