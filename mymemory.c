#include <stdio.h>
#include <stdlib.h>
#include "mymemory.h"

mymemory_t* mymemory_init(size_t size) {
    mymemory_t *memory = malloc(sizeof(mymemory_t));

    memory->pool = malloc(size);
    if (memory->pool == NULL) {
        printf("Erro ao criar pool de memoria.\n");
        free(memory);
        return NULL;
    }

    memory->total_size = size;
    memory->head = NULL; // lista de alocações começa vazia

    printf("Pool de memoria criada. \n");
    return memory;
}

void* mymemory_alloc(mymemory_t *memory, size_t size) {
    void *pool_start = memory->pool;
    void *pool_end = memory->pool + memory->total_size;

    allocation_t *atual = memory->head;
    allocation_t *anterior = NULL;

    void *posicao = pool_start;

    // procura o primeiro espaço livre
    while (atual != NULL) {
        void *inicio_bloco = atual->start;
        size_t espaco_livre = inicio_bloco - posicao;

        if (espaco_livre >= size) {
            break;
        }

        posicao = inicio_bloco + atual->size;
        anterior = atual;
        atual = atual->next;
    }

    if (pool_end - posicao < size) {
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

void mymemory_free(mymemory_t *memory, void *ptr) {
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

void mymemory_display(mymemory_t *memory) {
    printf("\n--- Pool ---\n");
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

void mymemory_cleanup(mymemory_t *memory) {
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
        printf("Memoria livre: %zu bytes\n", memory->total_size);
        printf("Maior bloco livre: %zu bytes\n", memory->total_size);
        printf("Fragmentos livres: 1\n");
        printf("------------------------------\n");
        return;
    }

    allocation_t *sorted = NULL;
    allocation_t *atual = memory->head;

    // faz o sort
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
    void *pool_end = memory->pool + memory->total_size;

    allocation_t *node = sorted;

    while (node != NULL) {
        num_alocacoes++;
        total_alocado += node->size;

        size_t espaco_livre = node->start - pos;
        if (espaco_livre > 0) {
            fragmentos_livres++;
            total_livre += espaco_livre;
            if (espaco_livre > maior_livre)
                maior_livre = espaco_livre;
        }

        pos = node->start + node->size;
        node = node->next;
    }

    size_t espaco_final = pool_end - pos;
    if (espaco_final > 0) {
        fragmentos_livres++;
        total_livre += espaco_final;
        if (espaco_final > maior_livre)
            maior_livre = espaco_final;
    }

    printf("\n--- Estatísticas da Memória ---\n");
    printf("Número de alocações feitas: %d\n", num_alocacoes);
    printf("Total de memoria alocada: %zu bytes\n", total_alocado);
    printf("Total de memoria livre: %zu bytes\n", total_livre);
    printf("Maior bloco livre: %zu bytes\n", maior_livre);
    printf("Fragmentos livres: %d\n", fragmentos_livres);
    printf("------------------------------\n");
}

