// main.cpp (secuencial) - mide solo el tiempo total (read+proc+write)
#include "PGM.h"
#include "PPM.h"
#include <iostream>
#include <cstring>
#include <time.h>

using namespace std;

static double now_seconds() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

int main(int argc, char* argv[]) {
    if (argc < 5) {
        cerr << "Uso: " << argv[0] << " <input> <output> --f <filtro>\n";
        cerr << "Filtros disponibles: blur, process, laplace, sharpening\n";
        return 1;
    }

    const char* inputFile  = argv[1];
    const char* outputFile = argv[2];
    const char* flag       = argv[3];
    const char* filter     = argv[4];

    if (strcmp(flag, "--f") != 0) {
        cerr << "Error: se esperaba --f\n";
        return 1;
    }

    string inFileStr(inputFile);
    bool isPGM = (inFileStr.find(".pgm") != string::npos);
    bool isPPM = (inFileStr.find(".ppm") != string::npos);

    if (!isPGM && !isPPM) {
        cerr << "Error: formato no soportado (solo .pgm y .ppm)\n";
        return 1;
    }

    double t0 = now_seconds();

    if (isPGM) {
        PGM img;
        img.setInputFile(inputFile);
        img.setOutputFile(outputFile);
        img.read();

        if (strcmp(filter, "blur") == 0) img.blur();
        else if (strcmp(filter, "process") == 0) img.process();
        else if (strcmp(filter, "laplace") == 0) img.laplace();
        else if (strcmp(filter, "sharpening") == 0) img.sharpening();
        else {
            cerr << "Filtro no válido para PGM: " << filter << "\n";
            return 1;
        }

        img.write();
    }
    else if (isPPM) {
        PPM img;
        img.setInputFile(inputFile);
        img.setOutputFile(outputFile);
        img.read();

        if (strcmp(filter, "blur") == 0) img.blur();
        else if (strcmp(filter, "process") == 0) img.process();
        else if (strcmp(filter, "laplace") == 0) img.laplace();
        else if (strcmp(filter, "sharpening") == 0) img.sharpening();
        else {
            cerr << "Filtro no válido para PPM: " << filter << "\n";
            return 1;
        }

        img.write();
    }

    double t1 = now_seconds();
    printf("Tiempo total: %.6f s\n", t1 - t0);

    return 0;
}
