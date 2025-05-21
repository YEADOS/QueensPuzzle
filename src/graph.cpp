#include "graph.h"

// Queen is represented by 0
// Non-queen spot represented by -1

Graph::Graph() {
    // puzzle = {
    //     {1, 2, 2, 2, 2, 2, 2, 2, 2},
    //     {1, 1, 2, 3, 3, 3, 2, 2, 2},
    //     {1, 2, 2, 3, 4, 3, 2, 2, 2},
    //     {1, 7, 2, 4, 4, 4, 5, 5, 2},
    //     {1, 7, 4, 4, 4, 4, 4, 5, 2},
    //     {1, 7, 7, 6, 4, 6, 5, 5, 2},
    //     {1, 1, 1, 6, 4, 6, 2, 2, 2},
    //     {9, 8, 8, 6, 6, 6, 2, 2, 2},
    //     {9, 9, 8, 8, 8, 8, 8, 2, 2}
    // };
    // puzzle = {
    //     {1, 1, 1, 1, 1, 1, 1, 1},
    //     {1, 1, 2, 2, 2, 3, 1, 1},
    //     {1, 1, 2, 3, 2, 3, 1, 1},
    //     {4, 4, 3, 3, 3, 3, 5, 5},
    //     {4, 6, 6, 3, 3, 7, 7, 5},
    //     {4, 4, 6, 3, 3, 3, 7, 5},
    //     {8, 6, 6, 8, 3, 7, 7, 5},
    //     {8, 8, 8, 8, 3, 3, 5, 5}
    // };

    // 30/4/2025 LinkedIn Queens Game

    puzzle = {
        {1, 2, 2, 2, 2, 2, 2},
        {1, 1, 1, 1, 3, 3, 2},
        {1, 1, 4, 5, 5, 3, 2},
        {1, 6, 4, 4, 5, 2, 2},
        {1, 6, 6, 4, 4, 2, 2},
        {7, 7, 6, 6, 2, 2, 2},
        {7, 7, 2, 2, 2, 2, 2}
    };
    puzzleBackup = puzzle;
}


bool Graph::isValid(int row, int col) {

    int n = puzzle.size();
    // Queen = 0

    int currentColour = puzzle[row][col];

    // Check to see if the queen is in the same column
    for (int i = 0; i < row; i++) {
        if (puzzle[i][col] == 0) {
            return false;
        }
    }
    
    if (row > 0 && col > 0 && puzzle[row - 1][col - 1] == 0)
        return false;

    // Check diagonally adjacent (upper-right)
    if (row > 0 && col < n - 1 && puzzle[row - 1][col + 1] == 0)
        return false;

    for (int i = 0; i < row; ++i) {
        for (int j = 0; j < n; ++j) {
            if (puzzle[i][j] == 0 && puzzleBackup[i][j] == currentColour)
                return false;
        }
    }
    
    return true;


}

bool Graph::solvePuzzle(int row, int n) {


    if (row == n) {
        // All queens placed, print solution
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                if (puzzle[i][j] == 0)
                    std::cout << "Q ";
                else
                    std::cout << ". ";
            }
            std::cout << std::endl;
        }
        std::cout << "---------------------\n";
        return true; // Set to false if you want all solutions
    }

    for (int col = 0; col < n; ++col) {
        if (isValid(row, col)) {
            int originalColor = puzzle[row][col];
            puzzle[row][col] = 0;  // Place queen

            if (solvePuzzle(row + 1, n))
                return true; // Change to continue if you want all solutions

            puzzle[row][col] = originalColor; // Backtrack
        }
    }

    return false;
}

std::vector<std::vector<int>> Graph::getGraph() {
    return puzzle;
}


int Graph::getSize() {
    return puzzle.size();
}

int main() {

    Graph g;

    
    if (!g.solvePuzzle(0, g.getSize())) {
        std::cout << "No solution found.\n";
    }

    return 1;

}