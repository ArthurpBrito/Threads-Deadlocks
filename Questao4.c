#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t travaAcesso;
pthread_mutex_t travaContLeitores;
int leitores = 0;
int dadoCompartilhado = 0;

void* leitor(void *arg) {
    int id = *((int*) arg);

    while (1) {
        pthread_mutex_lock(&travaContLeitores);
        leitores++;
        if (leitores == 1) {
            pthread_mutex_lock(&travaAcesso);
        }
        pthread_mutex_unlock(&travaContLeitores);

        printf("[Leitor %d] Lendo dado: %d\n", id, dadoCompartilhado);
        sleep(1 + rand() % 3);

        pthread_mutex_lock(&travaContLeitores);
        leitores--;
        if (leitores == 0) {
            pthread_mutex_unlock(&travaAcesso);
        }
        pthread_mutex_unlock(&travaContLeitores);

        sleep(1 + rand() % 3);
    }
    return NULL;
}

void* escritor(void *arg) {
    int id = *((int*) arg);

    while (1) {
        pthread_mutex_lock(&travaAcesso);

        dadoCompartilhado = rand() % 100;
        printf("[Escritor %d] Escrevendo dado: %d\n", id, dadoCompartilhado);
        sleep(1 + rand() % 3);

        pthread_mutex_unlock(&travaAcesso);

        sleep(1 + rand() % 3);
    }
    return NULL;
}

int main() {
    int quantLeitores, quantEscritores;

    printf("Informe o numero de threads leitoras: ");
    scanf("%d", &quantLeitores);

    printf("Informe o numero de threads escritoras: ");
    scanf("%d", &quantEscritores);

    pthread_t thrLeitores[quantLeitores], thrEscritores[quantEscritores];
    int idLeitores[quantLeitores], idEscritores[quantEscritores];

    pthread_mutex_init(&travaAcesso, NULL);
    pthread_mutex_init(&travaContLeitores, NULL);

    for (int i = 0; i < quantLeitores; i++) {
        idLeitores[i] = i + 1;
        pthread_create(&thrLeitores[i], NULL, leitor, &idLeitores[i]);
    }

    for (int i = 0; i < quantEscritores; i++) {
        idEscritores[i] = i + 1;
        pthread_create(&thrEscritores[i], NULL, escritor, &idEscritores[i]);
    }

    for (int i = 0; i < quantLeitores; i++) {
        pthread_join(thrLeitores[i], NULL);
    }
    for (int i = 0; i < quantEscritores; i++) {
        pthread_join(thrEscritores[i], NULL);
    }

    pthread_mutex_destroy(&travaAcesso);
    pthread_mutex_destroy(&travaContLeitores);

    return 0;
}
