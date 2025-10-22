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

int nextPowerOfTwo(int n) {
    if (n == 0) return 1;
    int power = 1;
    while (power < n) power <<= 1;
    return power;
}

void parallelPrefixSum(vector<int>& data) {
    int originalSize = static_cast<int>(data.size());
    int n = nextPowerOfTwo(originalSize);
    if (n != originalSize) {
        data.resize(n, 0);
    }

    upsweep(data, n);
    downsweep(data, n);

    for (int i = n - 1; i > 0; --i) {
        data[i] = data[i - 1];
    }
    data[0] = 0;

    for (int i = 1; i < n; ++i) {
        data[i] += data[i - 1];
    }

    data.resize(originalSize);
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
