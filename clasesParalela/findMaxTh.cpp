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
        cerr << "Error: array size must be a power of two." << endl;
        return;
    }

    upsweep(data, n);
    downsweep(data, n);
}

int main() {
    auto start = high_resolution_clock::now();

    int n;
    cin >> n;
    vector<int> data(n);
    for (int i = 0; i < n; ++i) {
        cin >> data[i];
    }

    parallelPrefixSum(data);

    for (int i = 0; i < n; ++i) {
        cout << data[i];
        if (i < n - 1) cout << " ";
    }
    cout << endl;

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    cout << "Tiempo total de ejecución: " << duration.count() << " microsegundos" << endl;

    return 0;
}
