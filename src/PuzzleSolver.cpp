#include "../include/PuzzleSolver.h"
#include "../include/graph.h"

PuzzleSolver::PuzzleSolver(Graph &graph) : puzzle(graph) {
    int queensPlaced = 0;
    int backtrackCount = 0;
    int probeCount = 0;
}

void PuzzleSolver::probe(int row, int col) {

    probeCount++;
    puzzle.getMasked()[row][col] = puzzle.getOriginal()[row][col];
}

bool PuzzleSolver::isValid(int row, int col) {

    int n = puzzle.getOriginal().size();
    int currentColour = puzzle.getCurrentState()[row][col];

    // Check to see if the queen is in the same column
    for (int i = 0; i < row; i++) {
        if (puzzle.getCurrentState()[i][col] == 0) {
            return false;
        }
    }
    
    if (row > 0 && col > 0 && puzzle.getCurrentState()[row - 1][col - 1] == 0)
        return false;

    // Check diagonally adjacent (upper-right)
    if (row > 0 && col < n - 1 && puzzle.getCurrentState()[row - 1][col + 1] == 0)
        return false;

    for (int i = 0; i < row; ++i) {
        for (int j = 0; j < n; ++j) {
            if (puzzle.getCurrentState()[i][j] == 0 && puzzle.getMasked()[i][j] == currentColour) // add condition to check if there's a -1 meaning it is unsure of if its correct
                return false;
        }
    }

    // add active sensing 
    // if [i][j] == -1

    return true;


}

// bool PuzzleSolver::isValid(int row, int col) {

//     int n = puzzle.getOriginal().size();
//     int currentColour = puzzle.getCurrentState()[row][col];

//     // Check to see if the queen is in the same column
//     for (int i = 0; i < row; i++) {
//         if (puzzle.getCurrentState()[i][col] == 0) {
//             return false;
//         }
//     }
    
//     if (row > 0 && col > 0 && puzzle.getCurrentState()[row - 1][col - 1] == 0)
//         return false;

//     // Check diagonally adjacent (upper-right)
//     if (row > 0 && col < n - 1 && puzzle.getCurrentState()[row - 1][col + 1] == 0)
//         return false;

//     // 0, 0
//     // [0][0] [0][1] [0][2] [0][3] [0][4] [0][5] ... 
//     // 1       1       2       2       2      2        3       3       3


//     for (int i = 0; i < row; ++i) {
//         for (int j = 0; j < n; ++j) {
//             if (puzzle.getCurrentState()[i][j] == 0 && puzzle.getOriginal()[i][j] == currentColour) // add condition to check if there's a -1 meaning it is unsure of if its correct
//                 return false;
//         }
//     }

//     // add active sensing 
//     // if [i][j] == -1

//     return true;


// }

bool PuzzleSolver::solvePuzzle(int row, int n) {
  
    if (row == n) {
        puzzle.printGraph(puzzle.ORIGINAL);
        puzzle.printGraph(puzzle.CURRENT_SYMBOLS);
        
        
        return true; 
    }


    for (int col = 0; col < n; ++col) {

        if (puzzle.getMasked()[row][col] == -1) {
            probe(row, col);
            continue;
        }
        if (isValid(row, col)) {
            int originalColor = puzzle.getMasked()[row][col];
            puzzle.getCurrentState()[row][col] = 0;  // Place queen
            queensPlaced++;

            if (solvePuzzle(row + 1, n))
                return true; // Change to continue if you want all solutions

            puzzle.getCurrentState()[row][col] = originalColor; // Backtrack
            backtrackCount++;
        }

    }

    return false;
}

// bool PuzzleSolver::solvePuzzle(int row, int n) {
  
//     if (row == n) {
//         puzzle.printGraph(puzzle.ORIGINAL);
//         puzzle.printGraph(puzzle.CURRENT_SYMBOLS);
        
//         return true; 
//     }

//     for (int col = 0; col < n; ++col) {
//         if (isValid(row, col)) {
//             int originalColor = puzzle.getOriginal()[row][col];
//             puzzle.getCurrentState()[row][col] = 0;  // Place queen

//             if (solvePuzzle(row + 1, n))
//                 return true; // Change to continue if you want all solutions

//             puzzle.getCurrentState()[row][col] = originalColor; // Backtrack
//         }
//     }

//     return false;
// }

void PuzzleSolver::printStatistics() {
    std::cout << "\n=== Solver Statistics ===" << std::endl;
    std::cout << "Queen placements: " << queensPlaced << std::endl;
    std::cout << "Backtracks: " << backtrackCount << std::endl;
    std::cout << "Probe operations: " << probeCount << std::endl;
        
        // Calculate efficiency
    double efficiency = (double)queensPlaced / (queensPlaced + backtrackCount);
    std::cout << "Efficiency ratio: " << efficiency << std::endl;
}

