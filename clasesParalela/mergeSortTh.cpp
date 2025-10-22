#include <iostream>
#include <vector>
#include <future>
#include <cmath>
#include <chrono>

using namespace std;
using namespace std::chrono;

vector<int> mergeVectors(const vector<int> &left, const vector<int> &right) {
    vector<int> result;
    int i = 0;
    int j = 0;

    while (i < (int)left.size() && j < (int)right.size()) {
        if (left[i] <= right[j]) {
            result.push_back(left[i]);
            i = i + 1;
        } else {
            result.push_back(right[j]);
            j = j + 1;
        }
    }

    while (i < (int)left.size()) {
        result.push_back(left[i]);
        i = i + 1;
    }

    while (j < (int)right.size()) {
        result.push_back(right[j]);
        j = j + 1;
    }

    return result;
}

vector<int> parallelMergeSort(const vector<int> &arr) {
    if ((int)arr.size() > 1) {
        int mid = (int)floor((double)arr.size() / 2.0);
        vector<int> leftHalf(arr.begin(), arr.begin() + mid);
        vector<int> rightHalf(arr.begin() + mid, arr.end());

        future<vector<int>> leftFuture = async(launch::async, parallelMergeSort, leftHalf);
        future<vector<int>> rightFuture = async(launch::async, parallelMergeSort, rightHalf);

        vector<int> leftSorted = leftFuture.get();
        vector<int> rightSorted = rightFuture.get();

        return mergeVectors(leftSorted, rightSorted);
    } else {
        return arr;
    }
}

int main() {
    auto start = high_resolution_clock::now();

    int n;
    cin >> n;
    vector<int> arr(n);
    for (int i = 0; i < n; ++i) {
        cin >> arr[i];
    }

    arr = parallelMergeSort(arr);

    cout << "Sorted array: ";
    for (int i = 0; i < n; ++i) {
        cout << arr[i];
        if (i < n - 1) cout << " ";
    }
    cout << "\n";

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    cout << "Tiempo total de ejecución: " << duration.count() << " microsegundos" << endl;

    return 0;
}
