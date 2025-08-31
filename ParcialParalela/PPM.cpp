#include "PPM.h"
#include <fstream>
#include <climits>
#include <cstdlib>

PPM::PPM() {
    mat = nullptr;
    N = 0;
    M = 0;
    maxValue = 255;
    inputFile = nullptr;
    outputFile = nullptr;
}

PPM::~PPM() {
    if (mat) {
        for (int i = 0; i < M; i++)
            delete[] mat[i];
        delete[] mat;
    }
}

void PPM::setInputFile(const char* filename) {
    inputFile = filename;
}

void PPM::setOutputFile(const char* filename) {
    outputFile = filename;
}

static void skip_comments(std::ifstream& fin) {
    fin >> ws;
    while (fin.peek() == '#') {
        fin.ignore(INT_MAX, '\n');
        fin >> ws;
    }
}

void PPM::read() {
    if (!inputFile) {
        cerr << "Error: archivo de entrada no asignado\n";
        return;
    }

    ifstream fin(inputFile);
    if (!fin.is_open()) {
        cerr << "Error: no se pudo abrir " << inputFile << "\n";
        exit(1);
    }

    char tipo[3];
    fin >> tipo;
    if (tipo[0] != 'P' || tipo[1] != '3') {
        cerr << "Formato incorrecto, se esperaba P3 (PPM)." << endl;
        exit(1);
    }

    skip_comments(fin);
    fin >> N;
    skip_comments(fin);
    fin >> M;
    skip_comments(fin);
    fin >> maxValue;

    mat = new Pixel*[M];
    for (int i = 0; i < M; i++)
        mat[i] = new Pixel[N];

    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            fin >> mat[i][j].r >> mat[i][j].g >> mat[i][j].b;

    fin.close();
}

void PPM::process() {
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            int gris = (mat[i][j].r + mat[i][j].g + mat[i][j].b) / 3;
            mat[i][j].r = mat[i][j].g = mat[i][j].b = gris;
        }
    }
}

void PPM::write() {
    if (!outputFile) {
        cerr << "Error: archivo de salida no asignado\n";
        return;
    }

    ofstream fout(outputFile);
    if (!fout.is_open()) {
        cerr << "Error: no se pudo abrir " << outputFile << "\n";
        exit(1);
    }

    fout << "P3\n" << N << " " << M << "\n" << maxValue << "\n";
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            fout << mat[i][j].r << " "
                 << mat[i][j].g << " "
                 << mat[i][j].b << " ";
        }
        fout << "\n";
    }
    fout.close();
}


void PPM::blur() {
    Pixel **temp = new Pixel *[M];
    for (int i = 0; i < M; i++)
        temp[i] = new Pixel[N];

    int offset = 1;

    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            int rSum = 0, gSum = 0, bSum = 0;
            int count = 0;

            for (int di = -offset; di <= offset; di++) {
                for (int dj = -offset; dj <= offset; dj++) {
                    int ni = i + di;
                    int nj = j + dj;
                    if (ni >= 0 && ni < M && nj >= 0 && nj < N) {
                        rSum += mat[ni][nj].r;
                        gSum += mat[ni][nj].g;
                        bSum += mat[ni][nj].b;
                        count++;
                    }
                }
            }

            temp[i][j].r = rSum / count;
            temp[i][j].g = gSum / count;
            temp[i][j].b = bSum / count;
        }
    }

    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            mat[i][j] = temp[i][j];

    for (int i = 0; i < M; i++)
        delete[] temp[i];
    delete[] temp;
}

void PPM::sharpening() {
    Pixel **temp = new Pixel *[M];
    for (int i = 0; i < M; i++)
        temp[i] = new Pixel[N];

    int kernel[3][3] = {
        { 0, -1,  0},
        {-1,  5, -1},
        { 0, -1,  0}
    };

    int offset = 1;

    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            int rSum = 0, gSum = 0, bSum = 0;

            for (int di = -offset; di <= offset; di++) {
                for (int dj = -offset; dj <= offset; dj++) {
                    int ni = i + di;
                    int nj = j + dj;

                    if (ni >= 0 && ni < M && nj >= 0 && nj < N) {
                        rSum += mat[ni][nj].r * kernel[di + offset][dj + offset];
                        gSum += mat[ni][nj].g * kernel[di + offset][dj + offset];
                        bSum += mat[ni][nj].b * kernel[di + offset][dj + offset];
                    }
                }
            }

            temp[i][j].r = min(max(rSum, 0), 255);
            temp[i][j].g = min(max(gSum, 0), 255);
            temp[i][j].b = min(max(bSum, 0), 255);
        }
    }

    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            mat[i][j] = temp[i][j];

    for (int i = 0; i < M; i++)
        delete[] temp[i];
    delete[] temp;
}

void PPM::laplace() {
    Pixel **temp = new Pixel *[M];
    for (int i = 0; i < M; i++)
        temp[i] = new Pixel[N];

    int kernel[3][3] = {
        { 0, -1,  0},
        {-1,  4, -1},
        { 0, -1,  0}
    };

    int offset = 1;

    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            int rSum = 0, gSum = 0, bSum = 0;

            for (int di = -offset; di <= offset; di++) {
                for (int dj = -offset; dj <= offset; dj++) {
                    int ni = i + di;
                    int nj = j + dj;

                    if (ni >= 0 && ni < M && nj >= 0 && nj < N) {
                        rSum += mat[ni][nj].r * kernel[di + offset][dj + offset];
                        gSum += mat[ni][nj].g * kernel[di + offset][dj + offset];
                        bSum += mat[ni][nj].b * kernel[di + offset][dj + offset];
                    }
                }
            }

            temp[i][j].r = min(max(rSum, 0), 255);
            temp[i][j].g = min(max(gSum, 0), 255);
            temp[i][j].b = min(max(bSum, 0), 255);
        }
    }

    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            mat[i][j] = temp[i][j];

    for (int i = 0; i < M; i++)
        delete[] temp[i];
    delete[] temp;
}

void PPM::blur2(int startRow, int endRow, int startCol, int endCol, Pixel **src, Pixel **dest) {
    int M_local = M, N_local = N;
    for (int i = startRow; i < endRow; ++i) {
        for (int j = startCol; j < endCol; ++j) {
            int rsum=0, gsum=0, bsum=0, cnt=0;
            for (int di=-1; di<=1; ++di) {
                for (int dj=-1; dj<=1; ++dj) {
                    int ni = i+di, nj = j+dj;
                    if (ni>=0 && ni<M_local && nj>=0 && nj<N_local) {
                        rsum += src[ni][nj].r;
                        gsum += src[ni][nj].g;
                        bsum += src[ni][nj].b;
                        ++cnt;
                    }
                }
            }
            if (cnt==0) cnt=1;
            dest[i][j].r = std::min(std::max((int)(rsum/cnt + 0.5), 0), maxValue);
            dest[i][j].g = std::min(std::max((int)(gsum/cnt + 0.5), 0), maxValue);
            dest[i][j].b = std::min(std::max((int)(bsum/cnt + 0.5), 0), maxValue);
        }
    }
}

void PPM::laplace2(int startRow, int endRow, int startCol, int endCol, Pixel **src, Pixel **dest) {
    int k[3][3] = { {0,-1,0}, {-1,4,-1}, {0,-1,0} };
    int M_local = M, N_local = N;
    for (int i = startRow; i < endRow; ++i) {
        for (int j = startCol; j < endCol; ++j) {
            int rr=0, gg=0, bb=0;
            for (int di=-1; di<=1; ++di) {
                for (int dj=-1; dj<=1; ++dj) {
                    int ni=i+di, nj=j+dj;
                    if (ni>=0 && ni<M_local && nj>=0 && nj<N_local) {
                        int w = k[di+1][dj+1];
                        rr += src[ni][nj].r * w;
                        gg += src[ni][nj].g * w;
                        bb += src[ni][nj].b * w;
                    }
                }
            }
            dest[i][j].r = std::min(std::max(rr, 0), maxValue);
            dest[i][j].g = std::min(std::max(gg, 0), maxValue);
            dest[i][j].b = std::min(std::max(bb, 0), maxValue);
        }
    }
}

void PPM::sharpening2(int startRow, int endRow, int startCol, int endCol, Pixel **src, Pixel **dest) {
    int k[3][3] = { {0,-1,0}, {-1,5,-1}, {0,-1,0} };
    int M_local = M, N_local = N;
    for (int i = startRow; i < endRow; ++i) {
        for (int j = startCol; j < endCol; ++j) {
            int rr=0, gg=0, bb=0;
            for (int di=-1; di<=1; ++di) {
                for (int dj=-1; dj<=1; ++dj) {
                    int ni=i+di, nj=j+dj;
                    if (ni>=0 && ni<M_local && nj>=0 && nj<N_local) {
                        int w = k[di+1][dj+1];
                        rr += src[ni][nj].r * w;
                        gg += src[ni][nj].g * w;
                        bb += src[ni][nj].b * w;
                    }
                }
            }
            dest[i][j].r = std::min(std::max(rr, 0), maxValue);
            dest[i][j].g = std::min(std::max(gg, 0), maxValue);
            dest[i][j].b = std::min(std::max(bb, 0), maxValue);
        }
    }
}

void PPM::blur_mpi(int **srcR,int **srcG,int **srcB,
                   int **destR,int **destG,int **destB,
                   int rows,int cols){
    for(int i=1;i<rows-1;i++){
        for(int j=1;j<cols-1;j++){
            int sumR=0,sumG=0,sumB=0;
            for(int di=-1;di<=1;di++)
                for(int dj=-1;dj<=1;dj++){
                    sumR+=srcR[i+di][j+dj];
                    sumG+=srcG[i+di][j+dj];
                    sumB+=srcB[i+di][j+dj];
                }
            destR[i][j]=sumR/9;
            destG[i][j]=sumG/9;
            destB[i][j]=sumB/9;
        }
    }
}

void PPM::laplace_mpi(int **srcR,int **srcG,int **srcB,
                      int **destR,int **destG,int **destB,
                      int rows,int cols){
    int kernel[3][3]={{0,-1,0},{-1,4,-1},{0,-1,0}};
    for(int i=1;i<rows-1;i++){
        for(int j=1;j<cols-1;j++){
            int sumR=0,sumG=0,sumB=0;
            for(int di=-1;di<=1;di++)
                for(int dj=-1;dj<=1;dj++){
                    sumR+=kernel[di+1][dj+1]*srcR[i+di][j+dj];
                    sumG+=kernel[di+1][dj+1]*srcG[i+di][j+dj];
                    sumB+=kernel[di+1][dj+1]*srcB[i+di][j+dj];
                }
            destR[i][j]=min(255,max(0,srcR[i][j]+sumR));
            destG[i][j]=min(255,max(0,srcG[i][j]+sumG));
            destB[i][j]=min(255,max(0,srcB[i][j]+sumB));
        }
    }
}

void PPM::sharpening_mpi(int **srcR,int **srcG,int **srcB,
                         int **destR,int **destG,int **destB,
                         int rows,int cols){
    int kernel[3][3]={{0,-1,0},{-1,5,-1},{0,-1,0}};
    for(int i=1;i<rows-1;i++){
        for(int j=1;j<cols-1;j++){
            int sumR=0,sumG=0,sumB=0;
            for(int di=-1;di<=1;di++)
                for(int dj=-1;dj<=1;dj++){
                    sumR+=kernel[di+1][dj+1]*srcR[i+di][j+dj];
                    sumG+=kernel[di+1][dj+1]*srcG[i+di][j+dj];
                    sumB+=kernel[di+1][dj+1]*srcB[i+di][j+dj];
                }
            destR[i][j]=min(255,max(0,sumR));
            destG[i][j]=min(255,max(0,sumG));
            destB[i][j]=min(255,max(0,sumB));
        }
    }
}
