#include <iostream>
#include <vector>
#include <thread>
#include <cmath>
#include <future>
#include <chrono>

using namespace std;
using namespace std::chrono;

void upsweep(vector<int>& data, int n) {
    for (int d = 0; (1 << d) < n; ++d) {
        int step = 1 << (d + 1);
        vector<future<void>> futures;

        for (int i = 0; i < n; i += step) {
            futures.push_back(async(launch::async, [&, i, d]() {
                int ai = i + (1 << d) - 1;
                int bi = i + (1 << (d + 1)) - 1;
                if (bi < n) {
                    data[bi] += data[ai];
                }
            }));
        }

        for (size_t k = 0; k < futures.size(); ++k) {
            futures[k].get();
        }
    }
}

void downsweep(vector<int>& data, int n) {
    data[n - 1] = 0;

    for (int d = static_cast<int>(log2(n)) - 1; d >= 0; --d) {
        int step = 1 << (d + 1);
        vector<future<void>> futures;

        for (int i = 0; i < n; i += step) {
            futures.push_back(async(launch::async, [&, i, d]() {
                int ai = i + (1 << d) - 1;
                int bi = i + (1 << (d + 1)) - 1;
                if (bi < n) {
                    int t = data[ai];
                    data[ai] = data[bi];
                    data[bi] += t;
                }
            }));
        }

        for (size_t k = 0; k < futures.size(); ++k) {
            futures[k].get();
        }
    }
}

void parallelPrefixSum(vector<int>& data) {
    int n = static_cast<int>(data.size());

    if ((n & (n - 1)) != 0) {
        cerr << "Error: array size must be una potencia de 2." << endl;
        return;
    }

    upsweep(data, n);
    downsweep(data, n);
}

int main() {
    int n = 8;
    vector<int> data = {3, 1, 7, 0, 4, 1, 6, 3};

    cout << "Original array: ";
    for (int i = 0; i < n; ++i) {
        cout << data[i] << " ";
    }
    cout << endl;

    auto start = high_resolution_clock::now();

    parallelPrefixSum(data);

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);

    cout << "Prefix sum: ";
    for (int i = 0; i < n; ++i) {
        cout << data[i] << " ";
    }
    cout << endl;

    cout << "Tiempo de ejecución: " << duration.count() << " microsegundos" << endl;

    return 0;
}
