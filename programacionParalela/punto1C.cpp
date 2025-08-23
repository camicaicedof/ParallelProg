#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

#define HILOS 2
long numTrapecios = 1024; 
double limiteA = 0.0, limiteB = M_PI; 
double integral = 0.0;
pthread_mutex_t mutex;

void* metodoTrapecio(void* idHilo){
    long miHilo = (long) idHilo;
    long trapeciosPorHilo = numTrapecios / HILOS;
    double h = (limiteB - limiteA) / numTrapecios;
    double localA = limiteA + miHilo * trapeciosPorHilo * h;
    double localB = localA + trapeciosPorHilo * h;
    double integralLocal = (sin(localA) + sin(localB)) / 2.0;
    for (long i = 1; i < trapeciosPorHilo; i++) {
        double x = localA + i * h;
        integralLocal += sin(x);
    }
    integralLocal = integralLocal * h;
    pthread_mutex_lock(&mutex);
    integral += integralLocal;
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int main() {
    pthread_t hilos[HILOS];
    pthread_mutex_init(&mutex, NULL);
    for (long h = 0; h < HILOS; h++)
        pthread_create(&hilos[h], NULL, metodoTrapecio, (void*)h);
    for (int h = 0; h < HILOS; h++)
        pthread_join(hilos[h], NULL);
    printf("Integral aproximada en [%f, %f] = %f\n", limiteA, limiteB, integral);
    pthread_mutex_destroy(&mutex);
    return 0;
}
