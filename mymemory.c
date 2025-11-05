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

// Função para "alocar" dentro da pool
void* mymemory_alloc(mymemory_t *memory, size_t size) {
    if (memory == NULL) return NULL;

    size_t usado = 0;
    allocation_t *atual = memory->head;

    while (atual != NULL) {
        usado += atual->size;
        atual = atual->next;
    }

    if (usado + size > memory->total_size) {
        printf("Nao ha espaco suficiente na pool!\n");
        return NULL;
    }

    void *endereco = (char*)memory->pool + usado;

    allocation_t *nova = (allocation_t*) malloc(sizeof(allocation_t));
    nova->start = endereco;
    nova->size = size;
    nova->next = memory->head; // insere no início da lista
    memory->head = nova;

    printf("Alocou %zu bytes dentro da pool.\n", size);
    return endereco;
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
