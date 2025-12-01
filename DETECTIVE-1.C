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


// Estrutura: Pista (BST)
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

Suspeito* tabelaHash[HASH_SIZE];

int hashFunc(const char *s) {
    int sum = 0;
    for (const char *p = s; *p; ++p) sum += (unsigned char)(*p);
    return sum % HASH_SIZE;
}

void inicializarHash() {
    for (int i = 0; i < HASH_SIZE; ++i) tabelaHash[i] = NULL;
}

Suspeito* buscarSuspeito(const char *nome) {
    int h = hashFunc(nome);
    Suspeito *cur = tabelaHash[h];
    while (cur) {
        if (strcmp(cur->nome, nome) == 0) return cur;
        cur = cur->prox;
    }
    return NULL;
}

Suspeito* inserirSuspeito(const char *nome) {
    Suspeito *exist = buscarSuspeito(nome);
    if (exist) return exist;
    int h = hashFunc(nome);
    Suspeito *s = (Suspeito*) malloc(sizeof(Suspeito));
    s->nome = strdup(nome);
    s->pistas = NULL;
    s->contador = 0;
    s->prox = tabelaHash[h];
    tabelaHash[h] = s;
    return s;
}

void associarPistaASuspeito(const char *suspeitoNome, const char *textoPista) {
    Suspeito *s = inserirSuspeito(suspeitoNome);
    // verificar se pista já foi associada a esse suspeito (simples check)
    SuspeitoPista *iter = s->pistas;
    while (iter) {
        if (strcmp(iter->textoPista, textoPista) == 0) return; // já associada
        iter = iter->prox;
    }
    SuspeitoPista *np = (SuspeitoPista*) malloc(sizeof(SuspeitoPista));
    np->textoPista = strdup(textoPista);
    np->prox = s->pistas;
    s->pistas = np;
    s->contador += 1;
}

void listarSuspeitos() {
    printf("\nSuspeitos e pistas associadas:\n");
    for (int i = 0; i < HASH_SIZE; ++i) {
        Suspeito *cur = tabelaHash[i];
        while (cur) {
            printf(" * %s (pistas: %d)\n", cur->nome, cur->contador);
            SuspeitoPista *sp = cur->pistas;
            while (sp) {
                printf("    - %s\n", sp->textoPista);
                sp = sp->prox;
            }
            cur = cur->prox;
        }
    }
}

Suspeito* suspeitoMaisProvavel() {
    Suspeito *best = NULL;
    for (int i = 0; i < HASH_SIZE; ++i) {
        Suspeito *cur = tabelaHash[i];
        while (cur) {
            if (!best || cur->contador > best->contador) best = cur;
            cur = cur->prox;
        }
    }
    return best;
}

void liberarHash() {
    for (int i = 0; i < HASH_SIZE; ++i) {
        Suspeito *cur = tabelaHash[i];
        while (cur) {
            SuspeitoPista *sp = cur->pistas;
            while (sp) {
                SuspeitoPista *tmp = sp;
                sp = sp->prox;
                free(tmp->textoPista);
                free(tmp);
            }
            Suspeito *tmpS = cur;
            cur = cur->prox;
            free(tmpS->nome);
            free(tmpS);
        }
        tabelaHash[i] = NULL;
    }
}

// Functions para mapa / exploração
Sala* montarMapa() {
    // Construção fixa: Hall raiz, Esquerda: Biblioteca -> Sótão, Direita: Cozinha -> Porão
    Sala *hall = criarSala("Hall de Entrada");
    Sala *bibli = criarSala("Biblioteca");
    Sala *cozinha = criarSala("Cozinha");
    Sala *sotao = criarSala("Sotao");
    Sala *porao = criarSala("Porao");
    Sala *escritorio = criarSala("Escritorio");
    Sala *jardim = criarSala("Jardim");
    // conectando (árvore simples)
    hall->esq = bibli;
    hall->dir = cozinha;
    bibli->esq = sotao;
    bibli->dir = escritorio;
    cozinha->esq = porao;
    cozinha->dir = jardim;
    return hall;
}

// Função para, ao entrar numa sala
void aoEntrarNaSala(const char *nomeSala, Pista **rootPistas) {
    if (strcmp(nomeSala, "Biblioteca") == 0) {
        const char *p = "Livro de receitas marcado na pagina 42";
        *rootPistas = inserirPistaBST(*rootPistas, p);
        associarPistaASuspeito("Sr. Martins", p);
    } else if (strcmp(nomeSala, "Cozinha") == 0) {
        const char *p = "Panelas limpas com cheiro de perfume floral";
        *rootPistas = inserirPistaBST(*rootPistas, p);
        associarPistaASuspeito("Sra. Oliveira", p);
        associarPistaASuspeito("Sr. Martins", p); // ambos relacionados
    } else if (strcmp(nomeSala, "Sotao") == 0) {
        const char *p = "Pegadas pequeninas no poeira";
        *rootPistas = inserirPistaBST(*rootPistas, p);
        associarPistaASuspeito("Criança desconhecida", p);
    } else if (strcmp(nomeSala, "Porao") == 0) {
        const char *p = "Carta rasgada com iniciais 'R.O.'";
        *rootPistas = inserirPistaBST(*rootPistas, p);
        associarPistaASuspeito("Rafael Oliveira", p);
    } else if (strcmp(nomeSala, "Escritorio") == 0) {
        const char *p = "Assinatura faltando na folha de ponto";
        *rootPistas = inserirPistaBST(*rootPistas, p);
        associarPistaASuspeito("Sra. Oliveira", p);
    } else if (strcmp(nomeSala, "Jardim") == 0) {
        const char *p = "Rastro de terra molhada levando ao portao";
        *rootPistas = inserirPistaBST(*rootPistas, p);
        associarPistaASuspeito("Zelador", p);
    } else if (strcmp(nomeSala, "Hall de Entrada") == 0) {
        const char *p = "Relogio parado as 9:15";
        *rootPistas = inserirPistaBST(*rootPistas, p);
        // essa pista não aponta para suspeito direto
    }
}