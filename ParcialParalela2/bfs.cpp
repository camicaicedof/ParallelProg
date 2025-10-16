#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <chrono>
using namespace std;
using namespace std::chrono;

int bfs(string start, string goal, int &expanded) {
    if (start == goal) return 0;
    queue<pair<string, int>> q;
    vector<string> visited;
    q.push(make_pair(start, 0));
    visited.push_back(start);
    expanded = 0;
    while (!q.empty()) {
        string current = q.front().first;
        int cost = q.front().second;
        q.pop();
        expanded++;
        int pos = current.find('#');
        int row = pos / 4;
        int col = pos % 4;
        int drow[4] = {-1, 1, 0, 0};
        int dcol[4] = {0, 0, -1, 1};
        for (int i = 0; i < 4; i++) {
            int newRow = row + drow[i];
            int newCol = col + dcol[i];
            if (newRow >= 0 && newRow < 4 && newCol >= 0 && newCol < 4) {
                int newPos = newRow * 4 + newCol;
                string next = current;
                swap(next[newPos], next[pos]);
                bool seen = false;
                for (int j = 0; j < visited.size(); j++) if (visited[j] == next) seen = true;
                if (!seen) {
                    if (next == goal) return cost + 1;
                    visited.push_back(next);
                    q.push(make_pair(next, cost + 1));
                }
            }
        }
    }
    return -1;
}

int main() {
    string start; cin >> start;
    string goal = "ABCDEFGHIJKLMNO#";
    auto t1 = high_resolution_clock::now();
    int expanded = 0;
    int result = bfs(start, goal, expanded);
    auto t2 = high_resolution_clock::now();
    double elapsed = duration_cast<duration<double>>(t2 - t1).count();
    if (result == -1) cout << "UNSOLVABLE\n";
    else cout << result << endl;
    cout << "Nodos expandidos: " << expanded << endl;
    cout << "Tiempo (s): " << elapsed << endl;
}
