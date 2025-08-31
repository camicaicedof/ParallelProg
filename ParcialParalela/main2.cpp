#include <iostream>
#include <cstring>
#include <pthread.h>
#include <cstdlib>
#include <cctype>
#include "PGM.h"
#include "PPM.h"
using namespace std;

#include <time.h>
static double now_seconds() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

struct PGMArgs {
    PGM* img;
    int **src;
    int **dest;
    int r1, r2, c1, c2;
    int filterType; // 0=blur,1=laplace,2=sharpen
};

struct PPMArgs {
    PPM* img;
    Pixel **src;
    Pixel **dest;
    int r1, r2, c1, c2;
    int filterType;
};

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

void* pgmWorker(void* v) {
    PGMArgs* a = (PGMArgs*)v;
    if (a->filterType == 0) a->img->blur2(a->r1,a->r2,a->c1,a->c2,a->src,a->dest);
    else if (a->filterType == 1) a->img->laplace2(a->r1,a->r2,a->c1,a->c2,a->src,a->dest);
    else if (a->filterType == 2) a->img->sharpening2(a->r1,a->r2,a->c1,a->c2,a->src,a->dest);
    return nullptr;
}

void* ppmWorker(void* v) {
    PPMArgs* a = (PPMArgs*)v;
    if (a->filterType == 0) a->img->blur2(a->r1,a->r2,a->c1,a->c2,a->src,a->dest);
    else if (a->filterType == 1) a->img->laplace2(a->r1,a->r2,a->c1,a->c2,a->src,a->dest);
    else if (a->filterType == 2) a->img->sharpening2(a->r1,a->r2,a->c1,a->c2,a->src,a->dest);
    return nullptr;
}

int main(int argc, char* argv[]) {
    // formato: ./pth_filterer input output --f <blur|laplace|sharpen>
    if (!(argc == 5 && strcmp(argv[3],"--f")==0)) {
        cerr << "Uso: " << argv[0] << " <input> <output> --f <blur|laplace|sharpen>\n";
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

    double t0 = now_seconds(); 

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

        int midR = M/2;
        int midC = N/2;
        pthread_t threads[4];
        PGMArgs args[4];

        args[0] = { &img, src, dest, 0, midR, 0, midC, filterType };
        args[1] = { &img, src, dest, 0, midR, midC, N, filterType };
        args[2] = { &img, src, dest, midR, M, 0, midC, filterType };
        args[3] = { &img, src, dest, midR, M, midC, N, filterType };

        for (int t=0;t<4;++t) {
            if (pthread_create(&threads[t], nullptr, pgmWorker, &args[t]) != 0) {
                cerr << "Error creando hilo PGM " << t << "\n";
                for (int i=0;i<M;++i) { delete[] src[i]; delete[] dest[i]; }
                delete[] src; delete[] dest;
                return 1;
            }
        }
        for (int t=0;t<4;++t) pthread_join(threads[t], nullptr);
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

        int midR = M/2;
        int midC = N/2;
        pthread_t threads[4];
        PPMArgs args[4];

        args[0] = { &img, src, dest, 0, midR, 0, midC, filterType };
        args[1] = { &img, src, dest, 0, midR, midC, N, filterType };
        args[2] = { &img, src, dest, midR, M, 0, midC, filterType };
        args[3] = { &img, src, dest, midR, M, midC, N, filterType };

        for (int t=0;t<4;++t) {
            if (pthread_create(&threads[t], nullptr, ppmWorker, &args[t]) != 0) {
                cerr << "Error creando hilo PPM " << t << "\n";
                for (int i=0;i<M;++i) { delete[] src[i]; delete[] dest[i]; }
                delete[] src; delete[] dest;
                return 1;
            }
        }
        for (int t=0;t<4;++t) pthread_join(threads[t], nullptr);

        for (int i=0;i<M;++i) for (int j=0;j<N;++j) img.mat[i][j] = dest[i][j];

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
