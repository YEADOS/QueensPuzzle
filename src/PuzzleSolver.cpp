#include "../include/PuzzleSolver.h"
#include "../include/graph.h"
#include <set>

const int PuzzleSolver::directions[4][2] = {{-1,0}, {1,0}, {0,-1}, {0,1}};

PuzzleSolver::PuzzleSolver(Graph &graph) : puzzle(graph) {
    // int queensPlaced = 0;
    // int backtrackCount = 0;
    // int probeCount = 0;
    // int inferredCount = 0;
}

bool PuzzleSolver::solvePuzzle(int row, int n) {

    if (row == n) {return true;} 

    for (int col = 0; col < n; ++col) {
        if (puzzle.getMasked()[row][col] == -1) { // Check to see if square is unknown
            int inferredColour = inferNeighbours(row, col);
            if (inferredColour != -1) { // If we can infer the colour
                puzzle.getMasked()[row][col] = inferredColour;
                inferredCount++;
                std::cout << "Inferred color " << inferredColour << " at (" << row << "," << col << ")\n";
            } else {
                probe(row, col); // Probe
                // continue; 

            }

        }

        if (isValid(row, col)) {
            int originalColor = puzzle.getMasked()[row][col];
            puzzle.getCurrentState()[row][col] = 0;  // Place queen
            queensPlaced++;

            if (solvePuzzle(row + 1, n))
                return true; 

            puzzle.getCurrentState()[row][col] = originalColor; // Backtrack
            backtrackCount++;
        }
    }
    return false;
}

int PuzzleSolver::inferNeighbours(int row, int col) {
    int n = puzzle.getOriginal().size();
    std::set<int> neighbours;

    for (int i = 0; i < 4; i++) {
        int newRow = row + directions[i][0];
        int newCol = col + directions[i][1];

        if (newRow >= 0 && newRow < n && newCol >= 0 && newCol < n) {
            int neighbourColour = puzzle.getMasked()[newRow][newCol];
            if (neighbourColour != -1) { 
                neighbours.insert(neighbourColour);
            } else {
                return -1;
            }
        }
    }
    if (neighbours.size() == 1) { // if there's only one unique neighbour colour then we can infer and know the exact colour
        return *neighbours.begin();
    }
    return -1;

}

void PuzzleSolver::probe(int row, int col) {
    probeCount++;
    int colour = puzzle.getOriginal()[row][col];
    std::cout << "Probed color " << colour << " at (" << row << "," << col << ")\n";
    puzzle.getMasked()[row][col] = colour;
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
    
    // Check diagonally adjacent (lower-left)
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
    return true;


}


void PuzzleSolver::printStatistics() {
    std::cout << "\n=== Solver Statistics ===" << std::endl;
    std::cout << "Queen placements: " << queensPlaced << std::endl;
    std::cout << "Backtracks: " << backtrackCount << std::endl;
    std::cout << "Probe operations: " << probeCount << std::endl;
    std::cout << "Inferred operations: " << inferredCount << std::endl;
        
    // Calculate efficiency
    double efficiency = (double)queensPlaced / (queensPlaced + backtrackCount);
    std::cout << "Efficiency ratio: " << efficiency << std::endl;
}

