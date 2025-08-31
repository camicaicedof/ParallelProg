#ifndef PPM_H
#define PPM_H
#include <iostream>
using namespace std;

class Pixel {
public:
    int r;
    int g;
    int b;

    Pixel() {
        r = g = b = 0;
    }
    Pixel(int red, int green, int blue) {
        r = red;
        g = green;
        b = blue;
    }
};

class PPM {
public:
    Pixel **mat;
    int N;  // ancho
    int M;  // alto
    int maxValue;

    PPM();
    ~PPM();

    void setInputFile(const char* filename);
    void setOutputFile(const char* filename);

    void read();
    void process();
    void write();
    void blur();
    void laplace();
    void sharpening();
    void blur2(int startRow, int endRow, int startCol, int endCol, Pixel **src, Pixel **dest);
    void laplace2(int startRow, int endRow, int startCol, int endCol, Pixel **src, Pixel **dest);
    void sharpening2(int startRow, int endRow, int startCol, int endCol, Pixel **src, Pixel **dest);
    void blur_mpi(int **srcR,int **srcG,int **srcB,
                  int **destR,int **destG,int **destB,
                  int rows,int cols);

    void laplace_mpi(int **srcR,int **srcG,int **srcB,
                     int **destR,int **destG,int **destB,
                     int rows,int cols);

    void sharpening_mpi(int **srcR,int **srcG,int **srcB,
                        int **destR,int **destG,int **destB,
                        int rows,int cols);

private:
    const char* inputFile;
    const char* outputFile;
};

#endif
