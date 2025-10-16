#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <chrono>
using namespace std;
using namespace std::chrono;

class Node {
public:
    string state; int g; int f;
    Node(string s, int gValue, int fValue) { state = s; g = gValue; f = fValue; }
};

class Compare {
public:
    bool operator()(Node a, Node b) { return a.f > b.f; }
};

int heuristic(string current, string goal) {
    int wrong = 0;
    for (int i = 0; i < 16; i++)
        if (current[i] != goal[i] && current[i] != '#') wrong++;
    return wrong;
}

int aStar(string start, string goal, int &expanded) {
    if (start == goal) return 0;
    priority_queue<Node, vector<Node>, Compare> pq;
    vector<string> visited;
    int h = heuristic(start, goal);
    pq.push(Node(start, 0, h));
    visited.push_back(start);
    expanded = 0;
    while (!pq.empty()) {
        Node current = pq.top(); pq.pop();
        expanded++;
        string board = current.state;
        int g = current.g;
        if (board == goal) return g;
        int pos = board.find('#');
        int row = pos / 4; int col = pos % 4;
        int dRow[4] = {-1, 1, 0, 0}; int dCol[4] = {0, 0, -1, 1};
        for (int i = 0; i < 4; i++) {
            int newRow = row + dRow[i], newCol = col + dCol[i];
            if (newRow >= 0 && newRow < 4 && newCol >= 0 && newCol < 4) {
                int newPos = newRow * 4 + newCol;
                string next = board; swap(next[newPos], next[pos]);
                bool seen = false;
                for (int j = 0; j < visited.size(); j++) if (visited[j] == next) seen = true;
                if (!seen) {
                    int hNext = heuristic(next, goal);
                    pq.push(Node(next, g + 1, g + 1 + hNext));
                    visited.push_back(next);
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
    int result = aStar(start, goal, expanded);
    auto t2 = high_resolution_clock::now();
    double elapsed = duration_cast<duration<double>>(t2 - t1).count();
    if (result == -1) cout << "UNSOLVABLE\n";
    else cout << result << endl;
    cout << "Nodos expandidos: " << expanded << endl;
    cout << "Tiempo (s): " << elapsed << endl;
}
