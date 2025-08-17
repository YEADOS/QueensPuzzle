#include "../include/RoboticSolver.h"
#include <random>
#include <iostream>

std::vector<std::vector<int>> RoboticSolver::createMaskedMatrix(const std::vector<std::vector<int>>& original, double mask_prob) {
    std::vector<std::vector<int>> masked = original;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::bernoulli_distribution mask(mask_prob);

    for (size_t i = 0; i < masked.size(); ++i) {
        for (size_t j = 0; j < masked[i].size(); ++j) {
            if (mask(gen)) {
                masked[i][j] = -1;
            }
        }
    }
    return masked;
}

RoboticSolver::RoboticSolver(Graph& graph, double mask_prob)
    : boardGraph(graph),
      originalPuzzle(graph.getGraph()),
      colourMask(createMaskedMatrix(graph.getGraph(), mask_prob)),
      queenBoard(graph.getSize(), std::vector<int>(graph.getSize(), 0)) {}

int RoboticSolver::probe(int row, int col) {
    if (colourMask[row][col] == -1) {
        colourMask[row][col] = originalPuzzle[row][col];
    }
    return colourMask[row][col];
}


bool RoboticSolver::isSafe(int row, int col) {
    int n = queenBoard.size();
    int colour = probe(row, col);

    // Check column
    for (int i = 0; i < row; ++i)
        if (queenBoard[i][col]) return false;

    // Check upper-left diagonal
    for (int i = row - 1, j = col - 1; i >= 0 && j >= 0; --i, --j)
        if (queenBoard[i][j]) return false;

    // Check upper-right diagonal
    for (int i = row - 1, j = col + 1; i >= 0 && j < n; --i, ++j)
        if (queenBoard[i][j]) return false;

    // Check colour constraint
    for (int i = 0; i < row; ++i)
        for (int j = 0; j < n; ++j)
            if (queenBoard[i][j] && probe(i, j) == colour)
                return false;

    return true;
}

bool RoboticSolver::solveRecursive(int row) {
    int n = queenBoard.size();
    if (row == n) return true;
    for (int col = 0; col < n; ++col) {
        if (colourMask[row][col] == -1) continue; // Skip masked cells// Skip masked cells
        if (isSafe(row, col)) {
            // Place queen and recurse
            queenBoard[row][col] = 1;
            if (solveRecursive(row + 1))
                return true;
            queenBoard[row][col] = 0; // Backtrack
        }
    }
    return false;
}

bool RoboticSolver::solve() {
    return solveRecursive(0);
}

void RoboticSolver::printSolution() {
    int n = queenBoard.size();
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j)
            std::cout << (queenBoard[i][j] ? "Q " : ". ");
        std::cout << "\n";
    }
}

void RoboticSolver::printMaskedMatrix() {
    int n = colourMask.size();
    std::cout << "Masked Matrix:\n";
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (colourMask[i][j] == -1) {
                std::cout << "? ";
            } else {
                std::cout << colourMask[i][j] << " ";
            }
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}