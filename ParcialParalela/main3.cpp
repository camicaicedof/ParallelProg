#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cctype>
#include <algorithm>
#include <omp.h>
#include "PGM.h"
#include "PPM.h"
#include <time.h>
using namespace std;

static double now_seconds() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

inline bool endsWithCI(const char* s, const char* ext) {
    int ls = strlen(s), le = strlen(ext);
    if (ls < le) return false;
    const char* p = s + (ls - le);
    for (int i=0;i<le;i++) {
        char a = tolower((unsigned char)p[i]);
        char b = tolower((unsigned char)ext[i]);
        if (a!=b) return false;
    }
    return true;
}

int main(int argc, char* argv[]) {
    if (!(argc == 5 && strcmp(argv[3],"--f")==0)) {
        return 1;
    }

    const char* inputFile = argv[1];
    const char* outputFile = argv[2];
    const char* filter = argv[4];

    int filterType = -1;
    if (strcmp(filter,"blur")==0) filterType = 0;
    else if (strcmp(filter,"laplace")==0) filterType = 1;
    else if (strcmp(filter,"sharpen")==0 || strcmp(filter,"sharpening")==0) filterType = 2;
    else {
        cerr << "Filtro no soportado: " << filter << "\n";
        return 1;
    }

    double t0 = now_seconds(); // inicio total

    if (endsWithCI(inputFile, ".pgm")) {
        PGM img;
        img.setInputFile(inputFile);
        img.setOutputFile(outputFile);
        img.read();

        int M = img.M;
        int N = img.N;

        int **src = new int*[M];
        int **dest = new int*[M];
        for (int i=0;i<M;++i) {
            src[i] = new int[N];
            dest[i] = new int[N];
            for (int j=0;j<N;++j) {
                src[i][j] = img.mat[i][j];
                dest[i][j] = img.mat[i][j];
            }
        }

        if (filterType == 0) { // BLUR
            #pragma omp parallel for collapse(2) schedule(static)
            for (int i=0;i<M;++i) {
                for (int j=0;j<N;++j) {
                    int sum = 0, cnt = 0;
                    for (int di=-1; di<=1; ++di) {
                        for (int dj=-1; dj<=1; ++dj) {
                            int ni = i + di, nj = j + dj;
                            if (ni >= 0 && ni < M && nj >= 0 && nj < N) {
                                sum += src[ni][nj];
                                ++cnt;
                            }
                        }
                    }
                    dest[i][j] = (cnt>0) ? (int)(sum / cnt + 0.5) : src[i][j];
                }
            }
        }
        else if (filterType == 1) { // LAPLACE
            int k[3][3] = { {0,-1,0}, {-1,4,-1}, {0,-1,0} };
            #pragma omp parallel for collapse(2) schedule(static)
            for (int i=0;i<M;++i) {
                for (int j=0;j<N;++j) {
                    int sum = 0;
                    for (int di=-1; di<=1; ++di)
                        for (int dj=-1; dj<=1; ++dj) {
                            int ni = i + di, nj = j + dj;
                            if (ni >= 0 && ni < M && nj >= 0 && nj < N)
                                sum += src[ni][nj] * k[di+1][dj+1];
                        }
                    sum = max(0, min(sum, img.maxValue));
                    dest[i][j] = sum;
                }
            }
        }
        else { // SHARPEN
            int k[3][3] = { {0,-1,0}, {-1,5,-1}, {0,-1,0} };
            #pragma omp parallel for collapse(2) schedule(static)
            for (int i=0;i<M;++i) {
                for (int j=0;j<N;++j) {
                    int sum = 0;
                    for (int di=-1; di<=1; ++di)
                        for (int dj=-1; dj<=1; ++dj) {
                            int ni = i + di, nj = j + dj;
                            if (ni >= 0 && ni < M && nj >= 0 && nj < N)
                                sum += src[ni][nj] * k[di+1][dj+1];
                        }
                    sum = max(0, min(sum, img.maxValue));
                    dest[i][j] = sum;
                }
            }
        }

        for (int i=0;i<M;++i) for (int j=0;j<N;++j) img.mat[i][j] = dest[i][j];

        for (int i=0;i<M;++i) { delete[] src[i]; delete[] dest[i]; }
        delete[] src; delete[] dest;

        img.write();
    }
    else if (endsWithCI(inputFile, ".ppm")) {
        PPM img;
        img.setInputFile(inputFile);
        img.setOutputFile(outputFile);
        img.read();

        int M = img.M;
        int N = img.N;

        // copiar src y crear dest
        Pixel **src = new Pixel*[M];
        Pixel **dest = new Pixel*[M];
        for (int i=0;i<M;++i) {
            src[i] = new Pixel[N];
            dest[i] = new Pixel[N];
            for (int j=0;j<N;++j) {
                src[i][j] = img.mat[i][j];
                dest[i][j] = img.mat[i][j];
            }
        }

        if (filterType == 0) { // BLUR
            #pragma omp parallel for collapse(2) schedule(static)
            for (int i=0;i<M;++i) for (int j=0;j<N;++j) {
                int rsum=0, gsum=0, bsum=0, cnt=0;
                for (int di=-1; di<=1; ++di) for (int dj=-1; dj<=1; ++dj) {
                    int ni = i + di, nj = j + dj;
                    if (ni >= 0 && ni < M && nj >= 0 && nj < N) {
                        rsum += src[ni][nj].r;
                        gsum += src[ni][nj].g;
                        bsum += src[ni][nj].b;
                        ++cnt;
                    }
                }
                if (cnt==0) cnt=1;
                dest[i][j].r = min(max((int)(rsum/cnt + 0.5), 0), img.maxValue);
                dest[i][j].g = min(max((int)(gsum/cnt + 0.5), 0), img.maxValue);
                dest[i][j].b = min(max((int)(bsum/cnt + 0.5), 0), img.maxValue);
            }
        }
        else if (filterType == 1) { // LAPLACE
            int k[3][3] = { {0,-1,0}, {-1,4,-1}, {0,-1,0} };
            #pragma omp parallel for collapse(2) schedule(static)
            for (int i=0;i<M;++i) for (int j=0;j<N;++j) {
                int rr=0, gg=0, bb=0;
                for (int di=-1; di<=1; ++di) for (int dj=-1; dj<=1; ++dj) {
                    int ni = i + di, nj = j + dj;
                    if (ni >= 0 && ni < M && nj >= 0 && nj < N) {
                        int w = k[di+1][dj+1];
                        rr += src[ni][nj].r * w;
                        gg += src[ni][nj].g * w;
                        bb += src[ni][nj].b * w;
                    }
                }
                dest[i][j].r = min(max(rr, 0), img.maxValue);
                dest[i][j].g = min(max(gg, 0), img.maxValue);
                dest[i][j].b = min(max(bb, 0), img.maxValue);
            }
        }
        else { // SHARPEN
            int k[3][3] = { {0,-1,0}, {-1,5,-1}, {0,-1,0} };
            #pragma omp parallel for collapse(2) schedule(static)
            for (int i=0;i<M;++i) for (int j=0;j<N;++j) {
                int rr=0, gg=0, bb=0;
                for (int di=-1; di<=1; ++di) for (int dj=-1; dj<=1; ++dj) {
                    int ni = i + di, nj = j + dj;
                    if (ni >= 0 && ni < M && nj >= 0 && nj < N) {
                        int w = k[di+1][dj+1];
                        rr += src[ni][nj].r * w;
                        gg += src[ni][nj].g * w;
                        bb += src[ni][nj].b * w;
                    }
                }
                dest[i][j].r = min(max(rr, 0), img.maxValue);
                dest[i][j].g = min(max(gg, 0), img.maxValue);
                dest[i][j].b = min(max(bb, 0), img.maxValue);
            }
        }

        // copiar dest -> img.mat
        for (int i=0;i<M;++i) for (int j=0;j<N;++j) img.mat[i][j] = dest[i][j];

        // liberar temp
        for (int i=0;i<M;++i) { delete[] src[i]; delete[] dest[i]; }
        delete[] src; delete[] dest;

        img.write();
    }
    else {
        cerr << "Formato no soportado. Usa .pgm o .ppm\n";
        return 1;
    }

    double t1 = now_seconds(); // fin total
    printf("Tiempo total: %.6f s\n", t1 - t0);

    return 0;
}
