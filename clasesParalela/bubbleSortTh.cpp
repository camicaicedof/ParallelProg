#include <iostream>
#include <vector>
#include <future>
#include <atomic>
#include <algorithm>
#include <chrono>

using namespace std;
using namespace std::chrono;

int main() {
    auto start = high_resolution_clock::now();

    int N;
    cin >> N;

    vector<int> A(N);
    for (int i = 0; i < N; ++i) {
        cin >> A[i];
    }

    if (N < 2) {
        for (int i = 0; i < N; ++i) {
            if (i) cout << " ";
            cout << A[i];
        }
        cout << "\n";
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        cout << "Tiempo total de ejecución: " << duration.count() << " microsegundos" << endl;
        return 0;
    }

    bool sorted = false;

    while (sorted == false) {
        sorted = true;
        atomic<bool> mark_changed(false);

        vector<future<void>> futures_odd;
        for (int i = 1; i + 1 < N; i += 2) {
            futures_odd.push_back(async(launch::async, [&A, &mark_changed, i]() {
                if (A[i] > A[i + 1]) {
                    swap(A[i], A[i + 1]);
                    mark_changed.store(true);
                }
            }));
        }

        for (size_t j = 0; j < futures_odd.size(); ++j) {
            futures_odd[j].get();
        }

        if (mark_changed.load() == true) {
            sorted = false;
        }

        mark_changed.store(false);
        vector<future<void>> futures_even;
        for (int i = 0; i + 1 < N; i += 2) {
            futures_even.push_back(async(launch::async, [&A, &mark_changed, i]() {
                if (A[i] > A[i + 1]) {
                    swap(A[i], A[i + 1]);
                    mark_changed.store(true);
                }
            }));
        }

        for (size_t j = 0; j < futures_even.size(); ++j) {
            futures_even[j].get();
        }

        if (mark_changed.load() == true) {
            sorted = false;
        }
    }

    cout << "Sorted array: ";
    for (int i = 0; i < N; ++i) {
        cout << A[i];
        if (i < N - 1) cout << " ";
    }
    cout << "\n";

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    cout << "Tiempo total de ejecución: " << duration.count() << " microsegundos" << endl;

    return 0;
}
