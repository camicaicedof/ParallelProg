#include <iostream>
#include <string>
#include <vector>
using namespace std;

vector<vector<char>> organizeBoard(string board){
    vector<vector<char>> organizedBoard;
    int count = 0;
    for (int i = 0; i < 4; i++){
        vector<char> vec;
        for (int j = 0; j < 4; j++){
            vec.push_back(board[count]);
            count++;
        }
        organizedBoard.push_back(vec);
    }
    return organizedBoard;
}

void printBoard(vector<vector<char>> organizedBoard){
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            cout << organizedBoard[i][j] << " ";
        }
        cout << endl;
    }
}

void movesAvailable(vector<vector<char>> organizedBoard){
    bool flag = true;
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            if (organizedBoard[i][j] == '#'){
                if (i > 0){
                    cout << "UP" << endl;
                }
                if (i < 3){
                    cout << "DOWN" << endl;
                }
                if (j > 0){
                    cout << "LEFT" << endl;
                }
                if (j < 3){
                    cout << "RIGHT" << endl;
                }
            }       
        }
    }
}

int main(){
    string board;
    cin >> board;
    vector<vector<char>> organizedBoard = organizeBoard(board);
    movesAvailable(organizedBoard);
    return 0;
}