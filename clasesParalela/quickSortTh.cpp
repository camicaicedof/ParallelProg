#include <iostream>
#include <vector>
#include <future>
#include <chrono>

using namespace std;
using namespace std::chrono;

int partitionVector(vector<int> &a, int low, int high) {
    int pivot = a[high];
    int i = low - 1;
    for (int j = low; j < high; ++j) {
        if (a[j] <= pivot) {
            i = i + 1;
            int temp = a[i];
            a[i] = a[j];
            a[j] = temp;
        }
    }
    int temp = a[i + 1];
    a[i + 1] = a[high];
    a[high] = temp;
    return i + 1;
}

void parallelQuickSort(vector<int> &a, int low, int high) {
    if (low < high) {
        int pivotIndex = partitionVector(a, low, high);
        future<void> leftFuture = async(launch::async, parallelQuickSort, ref(a), low, pivotIndex - 1);
        future<void> rightFuture = async(launch::async, parallelQuickSort, ref(a), pivotIndex + 1, high);
        leftFuture.get();
        rightFuture.get();
    }
}

int main() {
    auto start = high_resolution_clock::now();

    int n;
    cin >> n;
    vector<int> a(n);
    for (int i = 0; i < n; ++i) cin >> a[i];

    parallelQuickSort(a, 0, n - 1);

    cout << "Sorted array: ";
    for (int i = 0; i < n; ++i) {
        cout << a[i];
        if (i < n - 1) cout << " ";
    }
    cout << "\n";

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    cout << "Tiempo total de ejecución: " << duration.count() << " microsegundos" << endl;

    return 0;
}
