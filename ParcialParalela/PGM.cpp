#include "PGM.h"
#include <fstream>
#include <climits>
#include <cstdlib>

PGM::PGM() {
    mat = nullptr;
    N = 0;
    M = 0;
    maxValue = 255;
    inputFile = nullptr;
    outputFile = nullptr;
}

PGM::~PGM() {
    if (mat) {
        for (int i = 0; i < M; i++)
            delete[] mat[i];
        delete[] mat;
    }
}

void PGM::setInputFile(const char* filename) {
    inputFile = filename;
}

void PGM::setOutputFile(const char* filename) {
    outputFile = filename;
}

static void skip_comments(std::ifstream& fin) {
    fin >> ws;
    while (fin.peek() == '#') {
        fin.ignore(INT_MAX, '\n');
        fin >> ws;
    }
}

void PGM::read() {
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
    if (tipo[0] != 'P' || tipo[1] != '2') {
        cerr << "Formato incorrecto, se esperaba P2 (PGM)." << endl;
        exit(1);
    }

    skip_comments(fin);
    fin >> N;
    skip_comments(fin);
    fin >> M;
    skip_comments(fin);
    fin >> maxValue;

    mat = new int*[M];
    for (int i = 0; i < M; i++)
        mat[i] = new int[N];

    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            fin >> mat[i][j];

    fin.close();
}

void PGM::process() {
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            mat[i][j] = maxValue - mat[i][j]; // negativo
        }
    }
}

void PGM::write() {
    if (!outputFile) {
        cerr << "Error: archivo de salida no asignado\n";
        return;
    }

    ofstream fout(outputFile);
    if (!fout.is_open()) {
        cerr << "Error: no se pudo abrir " << outputFile << "\n";
        exit(1);
    }

    fout << "P2\n" << N << " " << M << "\n" << maxValue << "\n";
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            fout << mat[i][j] << " ";
        }
        fout << "\n";
    }
    fout.close();
}

void PGM::blur() {
    int **temp = new int*[M];
    for (int i = 0; i < M; i++)
        temp[i] = new int[N];

    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            int sum = 0, count = 0;

            for (int di = -1; di <= 1; di++) {
                for (int dj = -1; dj <= 1; dj++) {
                    int ni = i + di;
                    int nj = j + dj;
                    if (ni >= 0 && ni < M && nj >= 0 && nj < N) {
                        sum += mat[ni][nj];
                        count++;
                    }
                }
            }
            temp[i][j] = sum / count;
        }
    }

    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            mat[i][j] = temp[i][j];

    for (int i = 0; i < M; i++)
        delete[] temp[i];
    delete[] temp;
}

void PGM::laplace() {
    int **temp = new int *[M];
    for (int i = 0; i < M; i++)
        temp[i] = new int[N];

    int kernel[3][3] = {
        { 0, -1,  0},
        {-1,  4, -1},
        { 0, -1,  0}
    };

    int offset = 1;

    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            int sum = 0;

            for (int di = -offset; di <= offset; di++) {
                for (int dj = -offset; dj <= offset; dj++) {
                    int ni = i + di;
                    int nj = j + dj;

                    if (ni >= 0 && ni < M && nj >= 0 && nj < N) {
                        sum += mat[ni][nj] * kernel[di + offset][dj + offset];
                    }
                }
            }

            int val = sum;
            if (val < 0) val = 0;
            if (val > 255) val = 255;
            temp[i][j] = val;
        }
    }

    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            mat[i][j] = temp[i][j];

    for (int i = 0; i < M; i++)
        delete[] temp[i];
    delete[] temp;
}

void PGM::sharpening() {
    int **temp = new int *[M];
    for (int i = 0; i < M; i++)
        temp[i] = new int[N];

    int kernel[3][3] = {
        { 0, -1,  0},
        {-1,  5, -1},
        { 0, -1,  0}
    };

    int offset = 1;

    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            int sum = 0;

            for (int di = -offset; di <= offset; di++) {
                for (int dj = -offset; dj <= offset; dj++) {
                    int ni = i + di;
                    int nj = j + dj;

                    if (ni >= 0 && ni < M && nj >= 0 && nj < N) {
                        sum += mat[ni][nj] * kernel[di + offset][dj + offset];
                    }
                }
            }

            int val = sum;
            if (val < 0) val = 0;
            if (val > 255) val = 255;
            temp[i][j] = val;
        }
    }

    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            mat[i][j] = temp[i][j];

    for (int i = 0; i < M; i++)
        delete[] temp[i];
    delete[] temp;
}

void PGM::blur2(int startRow, int endRow, int startCol, int endCol, int **src, int **dest) {
    int M_local = M;
    int N_local = N;
    for (int i = startRow; i < endRow; ++i) {
        for (int j = startCol; j < endCol; ++j) {
            int sum = 0, cnt = 0;
            for (int di = -1; di <= 1; ++di) {
                for (int dj = -1; dj <= 1; ++dj) {
                    int ni = i + di;
                    int nj = j + dj;
                    if (ni >= 0 && ni < M_local && nj >= 0 && nj < N_local) {
                        sum += src[ni][nj];
                        ++cnt;
                    }
                }
            }
            int val = (cnt>0) ? (int)(sum / cnt + 0.5) : src[i][j];
            if (val < 0) val = 0;
            if (val > maxValue) val = maxValue;
            dest[i][j] = val;
        }
    }
}

void PGM::laplace2(int startRow, int endRow, int startCol, int endCol, int **src, int **dest) {
    int kernel[3][3] = { {0,-1,0}, {-1,4,-1}, {0,-1,0} };
    int M_local = M;
    int N_local = N;
    for (int i = startRow; i < endRow; ++i) {
        for (int j = startCol; j < endCol; ++j) {
            int sum = 0;
            for (int di = -1; di <= 1; ++di) {
                for (int dj = -1; dj <= 1; ++dj) {
                    int ni = i + di;
                    int nj = j + dj;
                    if (ni >= 0 && ni < M_local && nj >= 0 && nj < N_local) {
                        sum += src[ni][nj] * kernel[di+1][dj+1];
                    }
                }
            }
            int val = std::min(std::max(sum, 0), maxValue);
            dest[i][j] = val;
        }
    }
}

void PGM::sharpening2(int startRow, int endRow, int startCol, int endCol, int **src, int **dest) {
    int kernel[3][3] = { {0,-1,0}, {-1,5,-1}, {0,-1,0} };
    int M_local = M;
    int N_local = N;
    for (int i = startRow; i < endRow; ++i) {
        for (int j = startCol; j < endCol; ++j) {
            int sum = 0;
            for (int di = -1; di <= 1; ++di) {
                for (int dj = -1; dj <= 1; ++dj) {
                    int ni = i + di;
                    int nj = j + dj;
                    if (ni >= 0 && ni < M_local && nj >= 0 && nj < N_local) {
                        sum += src[ni][nj] * kernel[di+1][dj+1];
                    }
                }
            }
            int val = std::min(std::max(sum, 0), maxValue);
            dest[i][j] = val;
        }
    }
}

void PGM::blur_mpi(int **src,int **dest,int rows,int cols){
    for(int i=1;i<rows-1;i++){
        for(int j=1;j<cols-1;j++){
            int sum=0;
            for(int di=-1;di<=1;di++)
                for(int dj=-1;dj<=1;dj++)
                    sum+=src[i+di][j+dj];
            dest[i][j]=sum/9;
        }
    }
}

void PGM::laplace_mpi(int **src,int **dest,int rows,int cols){
    int kernel[3][3]={{0,-1,0},{-1,4,-1},{0,-1,0}};
    for(int i=1;i<rows-1;i++){
        for(int j=1;j<cols-1;j++){
            int sum=0;
            for(int di=-1;di<=1;di++)
                for(int dj=-1;dj<=1;dj++)
                    sum+=kernel[di+1][dj+1]*src[i+di][j+dj];
            dest[i][j]=min(255,max(0,src[i][j]+sum));
        }
    }
}

void PGM::sharpening_mpi(int **src,int **dest,int rows,int cols){
    int kernel[3][3]={{0,-1,0},{-1,5,-1},{0,-1,0}};
    for(int i=1;i<rows-1;i++){
        for(int j=1;j<cols-1;j++){
            int sum=0;
            for(int di=-1;di<=1;di++)
                for(int dj=-1;dj<=1;dj++)
                    sum+=kernel[di+1][dj+1]*src[i+di][j+dj];
            dest[i][j]=min(255,max(0,sum));
        }
    }
}
