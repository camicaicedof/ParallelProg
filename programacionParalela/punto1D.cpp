#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define TAMAÑO 10
#define HILOS 2

int arreglo[TAMAÑO] = {4, 6, 5, 2, 0, 9, 8, 3, 1, 7};
int arregloOrdenado[TAMAÑO];
int totalHilos = HILOS;

void* ordenamientoConteo(void* idHilo) {
    long miHilo = (long) idHilo;
    int elementosPorHilo = TAMAÑO / totalHilos;
    int inicio = miHilo * elementosPorHilo;
    int fin = (miHilo + 1) * elementosPorHilo;
    for (int i = inicio; i < fin; i++) {
        int conteo = 0;
        for (int j = 0; j < TAMAÑO; j++) {
            if (arreglo[j] < arreglo[i] || (arreglo[j] == arreglo[i] && j < i))
                conteo++;
        }
        arregloOrdenado[conteo] = arreglo[i];
    }
    return NULL;
}

int main() {
    pthread_t hilos[HILOS];
    for (long h = 0; h < HILOS; h++)
        pthread_create(&hilos[h], NULL, ordenamientoConteo, (void*)h);
    for (int h = 0; h < HILOS; h++)
        pthread_join(hilos[h], NULL);
    printf("Arreglo ordenado:\n");
    for (int i = 0; i < TAMAÑO; i++)
        printf("%d ", arregloOrdenado[i]);
    printf("\n");
    return 0;
}
