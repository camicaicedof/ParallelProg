#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define FILAS 4  
#define COLUMNAS 4   
#define HILOS 2

double matriz[FILAS][COLUMNAS];
double vector[COLUMNAS];
double resultado[FILAS];
int totalHilos = HILOS;

void* multiplicarMatrizVector(void* idHilo) {
    long miHilo = (long) idHilo;
    int filasPorHilo = FILAS / totalHilos;
    int primeraFila = miHilo * filasPorHilo;
    int ultimaFila = (miHilo + 1) * filasPorHilo - 1;
    for (int i = primeraFila; i <= ultimaFila; i++) {
        resultado[i] = 0.0;
        for (int j = 0; j < COLUMNAS; j++)
            resultado[i] += matriz[i][j] * vector[j];
    }
    return NULL;
}

int main() {
    pthread_t hilos[HILOS];
    for (int i = 0; i < FILAS; i++)
        for (int j = 0; j < COLUMNAS; j++)
            matriz[i][j] = i + j;
    for (int i = 0; i < COLUMNAS; i++)
        vector[i] = 1.0;
    for (long h = 0; h < HILOS; h++)
        pthread_create(&hilos[h], NULL, multiplicarMatrizVector, (void*)h);
    for (int h = 0; h < HILOS; h++)
        pthread_join(hilos[h], NULL);
    printf("Resultado vector = matriz * vector :\n");
    for (int i = 0; i < FILAS; i++)
        printf("%f\n", resultado[i]);
    return 0;
}
