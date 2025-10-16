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


int main(){
      string board;
      cin >> board;
      vector<vector<char>> organizedBoard = organizeBoard(board);
      printBoard(organizedBoard);
      return 0;
}