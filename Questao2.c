#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t trava;
pthread_cond_t cond_vagas, cond_itens;

typedef struct {
    int *buf;
    int tam;
    int posIns;
    int posRem;
    int count;
} Buffer;

int proximaPos(int posAtual, int tam) {
    return (posAtual + 1) % tam;
}

void inicializa(Buffer *b, int tam) {
    b->buf = (int*)malloc(sizeof(int) * tam);
    b->tam = tam;
    b->posIns = 0;
    b->posRem = 0;
    b->count = 0;
}

void* consumidor(void *arg) {
    Buffer *b = (Buffer*) arg;
    int item;

    while (1) {
        pthread_mutex_lock(&trava);
        while (b->count == 0) {
            pthread_cond_wait(&cond_itens, &trava);
        }

        item = b->buf[b->posRem];
        b->posRem = proximaPos(b->posRem, b->tam);
        b->count--;
        printf("[Consumidor] Removeu: %d | Posição de remoção: %d\n", item, b->posRem);

        pthread_cond_signal(&cond_vagas);
        pthread_mutex_unlock(&trava);

        sleep(1 + rand() % 3);
    }
    return NULL;
}

void* produtor(void *arg) {
    Buffer *b = (Buffer*) arg;
    int item;

    while (1) {
        item = rand() % 100;

        pthread_mutex_lock(&trava);
        while (b->count == b->tam) {
            pthread_cond_wait(&cond_vagas, &trava);
        }

        b->buf[b->posIns] = item;
        printf("[Produtor] Inseriu: %d | Posicao de insercao: %d\n", item, b->posIns);
        b->posIns = proximaPos(b->posIns, b->tam);
        b->count++;

        pthread_cond_signal(&cond_itens);
        pthread_mutex_unlock(&trava);

        sleep(1 + rand() % 3);
    }
    return NULL;
}

int main() {
    int tamBuf, qtdProd, qtdCons;

    printf("Informe o tamanho do buffer: ");
    scanf("%d", &tamBuf);

    printf("Informe o numero de threads produtoras: ");
    scanf("%d", &qtdProd);

    printf("Informe o numero de threads consumidoras: ");
    scanf("%d", &qtdCons);

    Buffer bufCompartilhado;
    inicializa(&bufCompartilhado, tamBuf);

    pthread_mutex_init(&trava, NULL);
    pthread_cond_init(&cond_vagas, NULL);
    pthread_cond_init(&cond_itens, NULL);

    pthread_t thrProd[qtdProd], thrCons[qtdCons];

    for (int i = 0; i < qtdProd; i++) {
        pthread_create(&thrProd[i], NULL, produtor, &bufCompartilhado);
    }

    for (int i = 0; i < qtdCons; i++) {
        pthread_create(&thrCons[i], NULL, consumidor, &bufCompartilhado);
    }

    for (int i = 0; i < qtdProd; i++) {
        pthread_join(thrProd[i], NULL);
    }
    for (int i = 0; i < qtdCons; i++) {
        pthread_join(thrCons[i], NULL);
    }

    free(bufCompartilhado.buf);
    pthread_mutex_destroy(&trava);
    pthread_cond_destroy(&cond_vagas);
    pthread_cond_destroy(&cond_itens);

    return 0;
}
