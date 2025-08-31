#ifndef PGM_H
#define PGM_H
#include <iostream>
#include <algorithm>
using namespace std;

class PGM {
public:
    int **mat;
    int N;          // ancho
    int M;          // alto
    int maxValue;

    PGM();
    ~PGM();

    void setInputFile(const char* filename);
    void setOutputFile(const char* filename);

    void read();
    void process();
    void write();
    void blur();
    void laplace();
    void sharpening();
    void blur2(int startRow, int endRow, int startCol, int endCol, int **src, int **dest);
    void laplace2(int startRow, int endRow, int startCol, int endCol, int **src, int **dest);
    void sharpening2(int startRow, int endRow, int startCol, int endCol, int **src, int **dest);
    void blur_mpi(int **src,int **dest,int rows,int cols);
    void laplace_mpi(int **src,int **dest,int rows,int cols);
    void sharpening_mpi(int **src,int **dest,int rows,int cols);


private:
    const char* inputFile;
    const char* outputFile;
};

#endif
