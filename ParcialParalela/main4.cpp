#include "PGM.h"
#include "PPM.h"
#include <mpi.h>
#include <iostream>
#include <chrono>
#include <cstring>

using namespace std;

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc < 5) {
        if (rank == 0) {
            cerr << "Uso: " << argv[0] << " <input> <output> --f <filtro>\n";
        }
        MPI_Finalize();
        return 1;
    }

    const char* inputFile  = argv[1];
    const char* outputFile = argv[2];
    const char* flag       = argv[3];
    const char* filter     = argv[4];

    bool isPGM = (string(inputFile).find(".pgm") != string::npos);
    bool isPPM = (string(inputFile).find(".ppm") != string::npos);

    auto start = chrono::high_resolution_clock::now();

    if (isPGM) {
        PGM img;
        if (rank == 0) {
            img.setInputFile(inputFile);
            img.setOutputFile(outputFile);
            img.read();
        }

        int rows, cols;
        if (rank == 0) {
            rows = img.M;
            cols = img.N;
        }

        MPI_Bcast(&rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&cols, 1, MPI_INT, 0, MPI_COMM_WORLD);
        int **src = new int*[rows];
        int **dest = new int*[rows];
        for (int i = 0; i < rows; i++) {
            src[i] = new int[cols];
            dest[i] = new int[cols];
        }

        if (rank == 0) {
            for (int i = 0; i < rows; i++)
                for (int j = 0; j < cols; j++)
                    src[i][j] = img.mat[i][j];
        }

        if (strcmp(filter, "blur") == 0)
            img.blur_mpi(src, dest, rows, cols);
        else if (strcmp(filter, "laplace") == 0)
            img.laplace_mpi(src, dest, rows, cols);
        else if (strcmp(filter, "sharpening") == 0)
            img.sharpening_mpi(src, dest, rows, cols);

        if (rank == 0) {
            for (int i = 0; i < rows; i++)
                for (int j = 0; j < cols; j++)
                    img.mat[i][j] = dest[i][j];

            img.write();
        }
    }

    else if (isPPM) {
        PPM img;
        if (rank == 0) {
            img.setInputFile(inputFile);
            img.setOutputFile(outputFile);
            img.read();
        }

        int rows, cols;
        if (rank == 0) {
            rows = img.M;
            cols = img.N;
        }

        MPI_Bcast(&rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&cols, 1, MPI_INT, 0, MPI_COMM_WORLD);

        // reservar memoria para R,G,B
        int **srcR = new int*[rows], **srcG = new int*[rows], **srcB = new int*[rows];
        int **destR = new int*[rows], **destG = new int*[rows], **destB = new int*[rows];
        for (int i = 0; i < rows; i++) {
            srcR[i] = new int[cols]; srcG[i] = new int[cols]; srcB[i] = new int[cols];
            destR[i] = new int[cols]; destG[i] = new int[cols]; destB[i] = new int[cols];
        }

        if (rank == 0) {
            for (int i = 0; i < rows; i++)
                for (int j = 0; j < cols; j++) {
                    srcR[i][j] = img.mat[i][j].r;
                    srcG[i][j] = img.mat[i][j].g;
                    srcB[i][j] = img.mat[i][j].b;
                }
        }

        if (strcmp(filter, "blur") == 0)
            img.blur_mpi(srcR, srcG, srcB, destR, destG, destB, rows, cols);
        else if (strcmp(filter, "laplace") == 0)
            img.laplace_mpi(srcR, srcG, srcB, destR, destG, destB, rows, cols);
        else if (strcmp(filter, "sharpening") == 0)
            img.sharpening_mpi(srcR, srcG, srcB, destR, destG, destB, rows, cols);

        if (rank == 0) {
            for (int i = 0; i < rows; i++)
                for (int j = 0; j < cols; j++) {
                    img.mat[i][j].r = destR[i][j];
                    img.mat[i][j].g = destG[i][j];
                    img.mat[i][j].b = destB[i][j];
                }

            img.write();
        }
    }

    auto end = chrono::high_resolution_clock::now();
    double tiempo = chrono::duration<double>(end - start).count();

    if (rank == 0) {
        cout << "Tiempo total MPI: " << tiempo << " segundos" << endl;
    }

    MPI_Finalize();
    return 0;
}
