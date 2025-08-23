#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define size 10
#define count 20

int buffer[size];
int count = 0;  
int in = 0, out = 0;

pthread_mutex_t mutex;
pthread_cond_t nempty, nfull;

void* productor(void* arg) {
    for (int i = 0; i < count; i++) {
        pthread_mutex_lock(&mutex);
        while (count == size){
            pthread_cond_wait(&nfull, &mutex);
        }
        buffer[in] = i; 
        printf("El productor dió: %d\n", i);
        in = (in + 1) % size;
        count++;
        pthread_cond_signal(&nempty);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void* consumer(void* arg) {
    for (int i = 0; i < count; i++) {
        pthread_mutex_lock(&mutex);
        while (count == 0) 
            pthread_cond_wait(&nempty, &mutex);

        int item = buffer[out];
        printf("El onsumidor usó: %d\n", item);
        out = (out + 1) % size;
        count--;
        pthread_cond_signal(&nfull);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main() {
    pthread_t prod, cons;
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&nempty, NULL);
    pthread_cond_init(&nfull, NULL);
    pthread_create(&prod, NULL, productor, NULL);
    pthread_create(&cons, NULL, consumer, NULL);
    pthread_join(prod, NULL);
    pthread_join(cons, NULL);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&nempty);
    pthread_cond_destroy(&nfull);
    return 0;
}