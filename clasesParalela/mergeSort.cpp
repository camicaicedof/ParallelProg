#include <iostream>
#include <vector>
#include <chrono>

using namespace std;
using namespace std::chrono;

vector<int> MERGE(const vector<int> &L, const vector<int> &R) {
    vector<int> result;
    int i = 0;
    int j = 0;

    while (i < L.size() && j < R.size()) {
        if (L[i] <= R[j]) {
            result.push_back(L[i]);
            i = i + 1;
        } else {
            result.push_back(R[j]);
            j = j + 1;
        }
    }

    while (i < (int)L.size()) {
        result.push_back(L[i]);
        i = i + 1;
    }

    while (j < (int)R.size()) {
        result.push_back(R[j]);
        j = j + 1;
    }

    return result;
}

vector<int> MERGE_SORT(const vector<int> &A) {
    if ((int)A.size() <= 1) {
        return A;
    }

    int mid = (int)A.size() / 2;

    vector<int> left_half(A.begin(), A.begin() + mid);
    vector<int> right_half(A.begin() + mid, A.end());

    left_half = MERGE_SORT(left_half);
    right_half = MERGE_SORT(right_half);

    vector<int> merged = MERGE(left_half, right_half);
    return merged;
}

int main() {
    auto start = high_resolution_clock::now();

    int N;
    cin >> N;

    vector<int> A(N);
    for (int i = 0; i < N; ++i) {
        cin >> A[i];
    }

    A = MERGE_SORT(A);

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
