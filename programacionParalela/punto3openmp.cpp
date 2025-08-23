#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <omp.h>

using namespace std;

vector<vector<int>> matrices(int n, int m){
    vector<vector<int>> A(n, vector<int>(m));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            A[i][j] = rand() % 10;
        }
    }
    return A;
}

int main() {
    srand(time(0));

    int n = 6, m = 4, p = 5;  
    vector<vector<int>> A(n, vector<int>(m));
    vector<vector<int>> B(m, vector<int>(p));
    vector<vector<int>> C(n, vector<int>(p, 0));

    A = matrices(n, m);
    B = matrices(m, p);

    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < p; j++) {
            int sum = 0;
            for (int k = 0; k < m; k++) {
                sum += A[i][k] * B[k][j];
            }
            C[i][j] = static_cast<int>(sum);
        }
    }

    cout << "Matriz A:" << endl;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            cout << A[i][j] << " ";
        }
        cout << endl;
    }

    cout << endl <<"Matriz B:" << endl;
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < p; j++) {
            cout << B[i][j] << " ";
        }
        cout << endl;
    }

    cout << endl << "Matriz C = A * B:" << endl;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < p; j++) {
            cout << C[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;
    
    return 0;
}
