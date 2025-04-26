#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <math.h>

#define SIZE 8
std::vector<std::vector<int>> board(SIZE, std::vector<int>(SIZE, 0));



void printSolutionInt() {
    std::cout << "Solution found:" << std::endl;
    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < SIZE; ++j) {
            std::cout << (board[i][j] == 1 ? 'Q' : '.'); // Translate here
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}




bool isSafe(int row, int col) {
    // Check this row on left side
    for (int i = 0; i < col; i++)
        if (board[row][i] == 1)
            return false;

    // Check upper diagonal on left side
    for (int i = row, j = col; i >= 0 && j >= 0; i--, j--)
        if (board[i][j] == 1)
            return false;

    // Check lower diagonal on left side
    for (int i = row, j = col; j >= 0 && i < SIZE; i++, j--)
        if (board[i][j] == 1)
            return false;

    return true;

}


bool placeQueens(int col) {


    if(col == SIZE) return true;

    // Consider the row and try placing
    // queen in all columns one by one
    for(int i = 0; i < SIZE; i++){

        if(isSafe(i, col)){
            board[i][col] = 1;
            if(placeQueens(col + 1)) 
                return true;
            board[i][col] = 0;
        }
    }
    return false;
}

int main() {

    srand(time(0));
    int randomCol = rand() % SIZE;
    // int randomCol = rand() % (SIZE + 1);

    if (placeQueens(randomCol)) {
        printSolutionInt();
    } else {
        std::cout << "No solution exists" << std::endl;
    }


    return 0;

}
