#include <iostream>
#include <pthread.h>
#include <vector>
#include <algorithm> 

using namespace std;

vector<long long> vec;
long long ans = 0;
pthread_mutex_t mut;

struct Range {
    long long start;
    long long end;
};

void* sumeRange(void* arg) {
    Range* point = (Range*) arg;
    long long localCount = 0;
    for (long long i = point->start; i < point->end; i++) {
        localCount += vec[i];
    }
    pthread_mutex_lock(&mut);
    ans += localCount;
    pthread_mutex_unlock(&mut);
    return nullptr;
}

int main() {
    long long N = 1e7;
    vec.resize(N);  
    for (long long j = 0; j < N; j++) {
        vec[j] = j + 1;
    }

    long long Nthreads = 4;
    long long intervals = (N + Nthreads - 1) / Nthreads;

    pthread_mutex_init(&mut, nullptr);
    pthread_t threads[Nthreads];
    vector<Range> rangos(Nthreads);

    for (int i = 0; i < Nthreads; i++) {
        long long start = i * intervals;
        long long end = min(N, start + intervals);
        rangos[i] = {start, end};
        pthread_create(&threads[i], nullptr, sumeRange, &rangos[i]);
    }

    for (int i = 0; i < Nthreads; i++) {
        pthread_join(threads[i], nullptr);
    }

    pthread_mutex_destroy(&mut);

    cout << "Suma total = " << ans << endl;

    return 0;
}
