#include <iostream>
#include <vector>
#include <omp.h>  

using namespace std;

int main() {
    long long N = 1e7;
    vector<long long> vec(N);
    for (long long i = 0; i < N; i++) {
        vec[i] = i + 1;
    }

    long long sum = 0;
    #pragma omp parallel for reduction(+:sum)
    for (long long i = 0; i < N; i++) {
        sum += vec[i];
    }

    cout << "Suma total = " << sum << endl;

    return 0;
}
