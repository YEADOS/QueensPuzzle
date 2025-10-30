#include "include/PuzzleManager.h"
#include "include/graph.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <set>

// Pure CSP solver without any masking or active sensing
// Solves LinkedIn Queens puzzle with complete information

class CSPLinkedInSolver {
private:
    Graph& puzzle;
    int n;
    std::vector<std::vector<int>> board;  // Working board state
    std::vector<std::pair<int, int>> solution;  // Queen positions

    bool isValid(int row, int col) {
        int cellColor = puzzle.getOriginal()[row][col];

        // Check if another queen already placed in same column
        for (int r = 0; r < n; r++) {
            if (r != row && board[r][col] == 1) {
                return false;
            }
        }

        // Check diagonal adjacency (LinkedIn rule: only diagonal touching forbidden)
        for (int r = 0; r < n; r++) {
            for (int c = 0; c < n; c++) {
                if (board[r][c] == 1) {
                    // Check if diagonally adjacent
                    if (abs(row - r) == 1 && abs(col - c) == 1) {
                        return false;
                    }
                }
            }
        }

        // Check color constraint: no other queen in same color region
        for (int r = 0; r < n; r++) {
            for (int c = 0; c < n; c++) {
                if (board[r][c] == 1) {
                    if (puzzle.getOriginal()[r][c] == cellColor) {
                        return false;  // Another queen in same color
                    }
                }
            }
        }

        return true;
    }

    bool solveBacktrack(int row) {
        if (row == n) {
            return true;  // All queens placed
        }

        // Try each column in this row
        for (int col = 0; col < n; col++) {
            if (isValid(row, col)) {
                // Place queen
                board[row][col] = 1;
                solution.push_back({row, col});

                // Recurse
                if (solveBacktrack(row + 1)) {
                    return true;
                }

                // Backtrack
                board[row][col] = 0;
                solution.pop_back();
            }
        }

        return false;  // No solution from this state
    }

public:
    CSPLinkedInSolver(Graph& g) : puzzle(g), n(g.getSize()) {
        // Initialize empty board
        board.resize(n, std::vector<int>(n, 0));
    }

    bool solve() {
        solution.clear();
        return solveBacktrack(0);
    }

    std::vector<std::pair<int, int>> getSolution() const {
        return solution;
    }

    void printSolution() const {
        std::cout << "Solution (row,col): ";
        for (const auto& [r, c] : solution) {
            std::cout << "(" << r << "," << c << ") ";
        }
        std::cout << std::endl;
    }
};

int main() {
    std::string filename = "puzzles.txt";
    int numPuzzles = 100;
    std::vector<Graph> puzzles;

    PuzzleManager::loadFromFile(filename, numPuzzles, puzzles);

    std::cout << "=== CSP LinkedIn Queens Solver ===" << std::endl;
    std::cout << "Generating ground truth solutions for " << puzzles.size() << " puzzles...\n\n";

    std::ofstream outFile("solutions.txt");
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open solutions.txt for writing" << std::endl;
        return 1;
    }

    int solvedCount = 0;

    for (size_t idx = 0; idx < puzzles.size(); idx++) {
        Graph& puzzle = puzzles[idx];
        int n = puzzle.getSize();

        std::cout << "Puzzle " << (idx + 1) << " (" << n << "x" << n << ")... ";

        CSPLinkedInSolver solver(puzzle);
        bool solved = solver.solve();

        if (solved) {
            std::cout << "✅ SOLVED" << std::endl;
            solvedCount++;

            // Write solution to file
            outFile << "PUZZLE " << (idx + 1) << " SOLVED" << std::endl;
            outFile << "SIZE " << n << std::endl;

            auto solution = solver.getSolution();
            for (const auto& [r, c] : solution) {
                outFile << r << " " << c << std::endl;
            }
            outFile << "END" << std::endl;

        } else {
            std::cout << "❌ NO SOLUTION" << std::endl;

            // Write failure to file
            outFile << "PUZZLE " << (idx + 1) << " FAILED" << std::endl;
            outFile << "SIZE " << n << std::endl;
            outFile << "END" << std::endl;
        }
    }

    outFile.close();

    std::cout << "\n=== RESULTS ===" << std::endl;
    std::cout << "Solved: " << solvedCount << "/" << puzzles.size() << std::endl;
    std::cout << "Success rate: " << (double)solvedCount / puzzles.size() * 100 << "%" << std::endl;
    std::cout << "\nGround truth solutions saved to solutions.txt" << std::endl;

    return 0;
}
