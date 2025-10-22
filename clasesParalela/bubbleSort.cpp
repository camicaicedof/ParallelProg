#include <iostream>
#include <vector>
#include <chrono>

using namespace std;
using namespace std::chrono;

void bubbleSort(vector<int>& vec) {
    int temp;
    int N = static_cast<int>(vec.size());
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N - i - 1; j++) {
            if (vec[j] > vec[j + 1]) {
                temp = vec[j];
                vec[j] = vec[j + 1];
                vec[j + 1] = temp;
            }
        }
    }
    for (int i = 0; i < N; i++) {
        cout << vec[i];
        if (i < N - 1) cout << " ";
    }
    cout << endl;
}

int main() {
    int n;
    cin >> n;
    vector<int> vec(n);
    for (int i = 0; i < n; ++i) {
        cin >> vec[i];
    }

    auto start = high_resolution_clock::now();
    bubbleSort(vec);
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    cout << "Tiempo total de ejecución: " << duration.count() << " microsegundos" << endl;

    return 0;
}
