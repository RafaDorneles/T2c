#include <stdio.h>
#include <stdlib.h>
#include "mymemory.h"

// Função para criar a pool de memória
mymemory_t* mymemory_init(size_t size) {
    mymemory_t *memory = (mymemory_t*) malloc(sizeof(mymemory_t));
    if (memory == NULL) {
        printf("Erro ao criar estrutura de memoria.\n");
        return NULL;
    }

    memory->pool = malloc(size);
    if (memory->pool == NULL) {
        printf("Erro ao criar pool de memoria.\n");
        free(memory);
        return NULL;
    }

    memory->total_size = size;
    memory->head = NULL; // lista de alocações começa vazia

    printf("Pool de memoria criada com %zu bytes.\n", size);
    return memory;
}

void* mymemory_alloc(mymemory_t *memory, size_t size) {
    if (memory == NULL || size == 0) return NULL;

    void *pool_start = memory->pool;
    void *pool_end = (char*)memory->pool + memory->total_size;

    allocation_t *atual = memory->head;
    allocation_t *anterior = NULL;

    void *posicao = pool_start;

    // percorre a lista procurando o primeiro espaço livre
    while (atual != NULL) {
        void *inicio_bloco = atual->start;
        size_t espaco_livre = (char*)inicio_bloco - (char*)posicao;

        if (espaco_livre >= size) {
            break;
        }

        posicao = (char*)inicio_bloco + atual->size;
        anterior = atual;
        atual = atual->next;
    }

    if ((char*)pool_end - (char*)posicao < size) {
        printf("Nao ha espaco suficiente na pool!\n");
        return NULL;
    }

    allocation_t *nova = (allocation_t*) malloc(sizeof(allocation_t));
    nova->start = posicao;
    nova->size = size;
    nova->next = atual;

    if (anterior == NULL)
        memory->head = nova; // insere no início da lista
    else
        anterior->next = nova; // insere depois do anterior

    printf("Alocou %zu bytes dentro da pool (First Fit).\n", size);
    return posicao;
}

// Função para "liberar" um pedaço da pool
void mymemory_free(mymemory_t *memory, void *ptr) {
    if (memory == NULL || ptr == NULL) return;

    allocation_t *atual = memory->head;
    allocation_t *anterior = NULL;

    while (atual != NULL) {
        if (atual->start == ptr) {
            if (anterior == NULL) {
                memory->head = atual->next;
            } else {
                anterior->next = atual->next;
            }

            free(atual);
            printf("Memoria liberada dentro da pool.\n");
            return;
        }

        anterior = atual;
        atual = atual->next;
    }

    printf("Endereco nao encontrado dentro da pool.\n");
}

// Função para mostrar o estado da pool
void mymemory_display(mymemory_t *memory) {
    if (memory == NULL) return;

    printf("\n--- Estado da pool ---\n");
    printf("Tamanho total: %zu bytes\n", memory->total_size);

    allocation_t *atual = memory->head;
    int count = 0;

    while (atual != NULL) {
        printf("Bloco %d: %zu bytes em %p\n", count, atual->size, atual->start);
        count++;
        atual = atual->next;
    }

    if (count == 0)
        printf("Nenhum bloco alocado.\n");
    printf("----------------------\n");
}

// Função para liberar toda a pool
void mymemory_cleanup(mymemory_t *memory) {
    if (memory == NULL) return;

    allocation_t *atual = memory->head;
    while (atual != NULL) {
        allocation_t *temp = atual;
        atual = atual->next;
        free(temp);
    }

    free(memory->pool);
    free(memory);

    printf("Pool de memoria liberada.\n");
}

void mymemory_stats(mymemory_t *memory) {
    if (memory == NULL) return;

    size_t total_alocado = 0;
    size_t total_livre = 0;
    size_t maior_livre = 0;
    int num_alocacoes = 0;
    int fragmentos_livres = 0;

    // se não existe alocacoes
    if (memory->head == NULL) {
        printf("\n--- Estatísticas da Memória ---\n");
        printf("Nenhuma alocação feita.\n");
        printf("Memória total livre: %zu bytes\n", memory->total_size);
        printf("Maior bloco livre: %zu bytes\n", memory->total_size);
        printf("Fragmentos livres: 1\n");
        printf("------------------------------\n");
        return;
    }

    allocation_t *sorted = NULL;
    allocation_t *atual = memory->head;

    while (atual != NULL) {
        allocation_t *novo = atual;
        atual = atual->next;

        if (sorted == NULL || sorted->start > novo->start) {
            novo->next = sorted;
            sorted = novo;
        } else {
            allocation_t *temp = sorted;
            while (temp->next != NULL && temp->next->start < novo->start)
                temp = temp->next;
            novo->next = temp->next;
            temp->next = novo;
        }
    }

    void *pos = memory->pool;
    void *pool_end = (char*)memory->pool + memory->total_size;

    allocation_t *node = sorted;

    while (node != NULL) {
        num_alocacoes++;
        total_alocado += node->size;

        size_t espaco_livre = (char*)node->start - (char*)pos;
        if (espaco_livre > 0) {
            fragmentos_livres++;
            total_livre += espaco_livre;
            if (espaco_livre > maior_livre)
                maior_livre = espaco_livre;
        }

        pos = (char*)node->start + node->size;
        node = node->next;
    }

    size_t espaco_final = (char*)pool_end - (char*)pos;
    if (espaco_final > 0) {
        fragmentos_livres++;
        total_livre += espaco_final;
        if (espaco_final > maior_livre)
            maior_livre = espaco_final;
    }

    printf("\n--- Estatísticas da Memória ---\n");
    printf("Número total de alocações: %d\n", num_alocacoes);
    printf("Memória total alocada: %zu bytes\n", total_alocado);
    printf("Memória total livre: %zu bytes\n", total_livre);
    printf("Maior bloco livre: %zu bytes\n", maior_livre);
    printf("Fragmentos livres: %d\n", fragmentos_livres);
    printf("------------------------------\n");
}

