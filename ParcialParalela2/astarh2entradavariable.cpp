#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <cmath>
using namespace std;

class Node {
public:
    string state;
    int g; // costo real
    int f; // f = g + h

    Node(string s, int gValue, int fValue) {
        state = s;
        g = gValue;
        f = fValue;
    }
};

class Compare {
public:
    bool operator()(Node a, Node b) {
        return a.f > b.f;
    }
};

// Heurística h2: suma de distancias Manhattan
int heuristic(string current, string goal, int n) {
    int total = 0;
    for (int i = 0; i < current.size(); i++) {
        if (current[i] != '#' && current[i] != goal[i]) {
            char tile = current[i];
            int goalPos = goal.find(tile);
            int currentRow = i / n;
            int currentCol = i % n;
            int goalRow = goalPos / n;
            int goalCol = goalPos % n;
            total += abs(currentRow - goalRow) + abs(currentCol - goalCol);
        }
    }
    return total;
}

int aStar(string start, string goal, int n) {
    if (start == goal) {
        return 0;
    }

    priority_queue<Node, vector<Node>, Compare> pq;
    vector<string> visited;

    int h = heuristic(start, goal, n);
    Node first(start, 0, h);
    pq.push(first);
    visited.push_back(start);

    while (!pq.empty()) {
        Node current = pq.top();
        pq.pop();

        string board = current.state;
        int g = current.g;

        if (board == goal) {
            return g;
        }

        int pos = board.find('#');
        int row = pos / n;
        int col = pos % n;

        int dRow[4] = {-1, 1, 0, 0};
        int dCol[4] = {0, 0, -1, 1};

        for (int i = 0; i < 4; i++) {
            int newRow = row + dRow[i];
            int newCol = col + dCol[i];

            if (newRow >= 0 && newRow < n && newCol >= 0 && newCol < n) {
                int newPos = newRow * n + newCol;
                string next = board;
                char temp = next[newPos];
                next[newPos] = next[pos];
                next[pos] = temp;

                bool seen = false;
                for (int j = 0; j < visited.size(); j++) {
                    if (visited[j] == next) {
                        seen = true;
                    }
                }

                if (!seen) {
                    int hNext = heuristic(next, goal, n);
                    Node nextNode(next, g + 1, g + 1 + hNext);
                    pq.push(nextNode);
                    visited.push_back(next);
                }
            }
        }
    }

    return -1;
}

int main() {
    string start;
    cin >> start;

    int n = sqrt(start.size());
    string goal = "";

    // Generar estado objetivo dinámicamente
    for (int i = 0; i < start.size() - 1; i++) {
        goal += 'A' + i;
    }
    goal += '#';

    int result = aStar(start, goal, n);

    if (result == -1)
        cout << "UNSOLVABLE" << endl;
    else
        cout << result << endl;

    return 0;
}
