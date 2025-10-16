#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <chrono>
using namespace std;
using namespace std::chrono;

void bfsThread(string initialState, int initialG, const string& goal, int& sharedBest, mutex& mtx, int& expanded) {
    queue<pair<string, int>> q;
    vector<string> visited;
    q.push(make_pair(initialState, initialG));
    visited.push_back(initialState);

    while (!q.empty()) {
        pair<string, int> current = q.front();
        q.pop();
        expanded++;

        string board = current.first;
        int g = current.second;

        {
            lock_guard<mutex> lock(mtx);
            if (g >= sharedBest) continue;
        }

        if (board == goal) {
            lock_guard<mutex> lock(mtx);
            if (g < sharedBest) sharedBest = g;
            continue;
        }

        int pos = board.find('#');
        int row = pos / 4;
        int col = pos % 4;
        int dRow[4] = {-1, 1, 0, 0};
        int dCol[4] = {0, 0, -1, 1};

        for (int i = 0; i < 4; i++) {
            int newRow = row + dRow[i];
            int newCol = col + dCol[i];
            if (newRow >= 0 && newRow < 4 && newCol >= 0 && newCol < 4) {
                int newPos = newRow * 4 + newCol;
                string next = board;
                swap(next[newPos], next[pos]);

                bool seen = false;
                for (int j = 0; j < (int)visited.size(); j++) {
                    if (visited[j] == next) seen = true;
                }
                if (!seen) {
                    q.push(make_pair(next, g + 1));
                    visited.push_back(next);
                }
            }
        }
    }
}

int main() {
    string start;
    cin >> start;
    string goal = "ABCDEFGHIJKLMNO#";

    if (start == goal) {
        cout << 0 << endl;
        return 0;
    }

    int pos = start.find('#');
    int row = pos / 4;
    int col = pos % 4;

    int dRow[4] = {-1, 1, 0, 0};
    int dCol[4] = {0, 0, -1, 1};
    vector<string> initialStates;

    for (int i = 0; i < 4; i++) {
        int newRow = row + dRow[i];
        int newCol = col + dCol[i];
        if (newRow >= 0 && newRow < 4 && newCol >= 0 && newCol < 4) {
            int newPos = newRow * 4 + newCol;
            string next = start;
            swap(next[newPos], next[pos]);
            initialStates.push_back(next);
        }
    }

    int sharedBest = 1000000000;
    mutex mtx;
    vector<thread> threads;
    vector<int> expanded(initialStates.size(), 0);

    auto startTime = high_resolution_clock::now();

    for (int i = 0; i < (int)initialStates.size(); i++) {
        threads.push_back(thread(bfsThread, initialStates[i], 1, cref(goal), ref(sharedBest), ref(mtx), ref(expanded[i])));
    }

    for (auto& t : threads) t.join();

    auto endTime = high_resolution_clock::now();
    double elapsed = duration_cast<duration<double>>(endTime - startTime).count();

    cout << "Costo óptimo: ";
    if (sharedBest == 1000000000) cout << "UNSOLVABLE" << endl;
    else cout << sharedBest << endl;

    cout << "Tiempo de ejecución: " << elapsed << " segundos" << endl;

    double tSecuencial = 4.0; // remplaza por tiempo medido secuencial
    double speedup = tSecuencial / elapsed;
    double eficiencia = speedup / threads.size();

    cout << "Speedup: " << speedup << endl;
    cout << "Eficiencia: " << eficiencia << endl;

    for (int i = 0; i < (int)expanded.size(); i++)
        cout << "Nodos expandidos por hilo " << i+1 << ": " << expanded[i] << endl;
}
