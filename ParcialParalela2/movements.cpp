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

void moveBoard(vector<vector<char>> organizedBoard, string move){
    bool flag = true;
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            if (organizedBoard[i][j] == '#' and flag){
                if (move == "RIGHT" and j < 3){
                    organizedBoard[i][j] = organizedBoard[i][j+1];
                    organizedBoard[i][j+1] = '#';
                    flag = false;
                }
                else if (move == "LEFT" and j > 0){
                    organizedBoard[i][j] = organizedBoard[i][j-1];
                    organizedBoard[i][j-1] = '#';
                    flag = false;
                }
                else if (move == "UP" and i > 0){
                    organizedBoard[i][j] = organizedBoard[i-1][j];
                    organizedBoard[i-1][j] = '#';
                    flag = false;
                }
                else if (move == "DOWN" and i < 3){
                    organizedBoard[i][j] = organizedBoard[i+1][j];
                    organizedBoard[i+1][j] = '#';
                    flag = false;
                }
            }       
        }
    }
    printBoard(organizedBoard);
}

int main(){
    string board, move;
    cin >> board;
    cin >> move;
    vector<vector<char>> organizedBoard = organizeBoard(board);
    printBoard(organizedBoard);
    cout << endl;
    moveBoard(organizedBoard, move);
    return 0;
}