#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <chrono>
#include <cmath>
#include <map>

using namespace std;
using namespace std::chrono;

class Node {
public:
    string state;
    int g;
    int f;
    Node(string s, int gValue, int fValue) {
        state = s; g = gValue; f = fValue;
    }
};

class Compare {
public:
    bool operator()(const Node& a, const Node& b) {
        return a.f > b.f;
    }
};

// heurística h2: suma de distancias Manhattan (excluye '#')
int heuristicH2(const string& state, const string& goal) {
    int sum = 0;
    for (int i = 0; i < 16; i++) {
        char c = state[i];
        if (c == '#') continue;
        int goalPos = goal.find(c);
        int r1 = i / 4, c1 = i % 4;
        int r2 = goalPos / 4, c2 = goalPos % 4;
        sum += abs(r1 - r2) + abs(c1 - c2);
    }
    return sum;
}

void aStarH2Thread(string initialState, int initialG, const string& goal,
                   int& sharedBest, mutex& mtx, int& expanded) {
    priority_queue<Node, vector<Node>, Compare> pq;
    map<string,int> bestG;

    int h0 = heuristicH2(initialState, goal);
    pq.push(Node(initialState, initialG, initialG + h0));
    bestG[initialState] = initialG;

    while (!pq.empty()) {
        Node current = pq.top();
        pq.pop();

        expanded++;

        int localBest;
        {
            lock_guard<mutex> lock(mtx);
            localBest = sharedBest;
        }
        if (current.f >= localBest) continue;

        if (current.state == goal) {
            lock_guard<mutex> lock(mtx);
            if (current.g < sharedBest) sharedBest = current.g;
            continue;
        }

        int pos = current.state.find('#');
        int row = pos / 4;
        int col = pos % 4;
        int dRow[4] = {-1, 1, 0, 0};
        int dCol[4] = {0, 0, -1, 1};

        for (int i = 0; i < 4; i++) {
            int newRow = row + dRow[i];
            int newCol = col + dCol[i];
            if (newRow < 0 || newRow >= 4 || newCol < 0 || newCol >= 4) continue;

            int newPos = newRow * 4 + newCol;
            string next = current.state;
            char tmp = next[newPos];
            next[newPos] = next[pos];
            next[pos] = tmp;

            int gNext = current.g + 1;

            // mejor g por hilo
            map<string,int>::iterator it = bestG.find(next);
            if (it != bestG.end()) {
                if (gNext >= it->second) continue;
            }

            int hNext = heuristicH2(next, goal);
            int fNext = gNext + hNext;

            int currentBest;
            {
                lock_guard<mutex> lock(mtx);
                currentBest = sharedBest;
            }
            if (fNext >= currentBest) continue;

            bestG[next] = gNext;
            pq.push(Node(next, gNext, fNext));
        }
    }
}

int main() {
    string start;
    if (!(cin >> start)) return 0;

    if ((int)start.size() != 16) {
        cout << "UNSOLVABLE" << endl;
        return 0;
    }

    string goal = "ABCDEFGHIJKLMNO#";
    if (start == goal) {
        cout << 0 << endl;
        return 0;
    }

    int pos = start.find('#');
    if (pos == string::npos) {
        cout << "UNSOLVABLE" << endl;
        return 0;
    }

    int row = pos / 4, col = pos % 4;
    int dRow[4] = {-1, 1, 0, 0};
    int dCol[4] = {0, 0, -1, 1};
    vector<string> initialStates;
    for (int i = 0; i < 4; i++) {
        int newRow = row + dRow[i];
        int newCol = col + dCol[i];
        if (newRow < 0 || newRow >= 4 || newCol < 0 || newCol >= 4) continue;
        int newPos = newRow * 4 + newCol;
        string next = start;
        char tmp = next[newPos];
        next[newPos] = next[pos];
        next[pos] = tmp;
        initialStates.push_back(next);
    }

    int sharedBest = 1000000000;
    mutex mtx;
    vector<thread> threads;
    vector<int> expanded(initialStates.size(), 0);

    auto t0 = high_resolution_clock::now();

    for (int i = 0; i < (int)initialStates.size(); i++) {
        threads.push_back(thread(aStarH2Thread, initialStates[i], 1, cref(goal),
                                 ref(sharedBest), ref(mtx), ref(expanded[i])));
    }

    for (int i = 0; i < (int)threads.size(); i++) threads[i].join();

    auto t1 = high_resolution_clock::now();
    double elapsed = duration_cast<duration<double>>(t1 - t0).count();

    cout << "Costo optimo: ";
    if (sharedBest == 1000000000) cout << "UNSOLVABLE" << endl;
    else cout << sharedBest << endl;

    cout << "Tiempo ejecucion (s): " << elapsed << endl;
    double tSecuencial = 4.0; // reemplaza por tu medición secuencial real
    double speedup = (tSecuencial / elapsed);
    double eficiencia = speedup / (double)threads.size();
    cout << "Speedup: " << speedup << endl;
    cout << "Eficiencia: " << eficiencia << endl;

    for (int i = 0; i < (int)expanded.size(); i++) {
        cout << "Nodos expandidos por hilo " << (i+1) << ": " << expanded[i] << endl;
    }

    return 0;
}
