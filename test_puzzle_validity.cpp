#include <iostream>
#include <vector>
#include <set>
#include <cmath>

// Quick checker to see if original puzzles have valid queen placements
bool isValidSolution(const std::vector<std::vector<int>>& puzzle) {
    int n = puzzle.size();
    std::vector<std::pair<int, int>> queens;

    // Find all cells with value 0 (queens) - but original puzzles don't have queens marked
    // Instead, we need to check if the puzzle COULD have a valid solution
    // Check basic structure: count colors
    std::set<int> colors;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            colors.insert(puzzle[i][j]);
        }
    }

    std::cout << "Puzzle size: " << n << "x" << n << std::endl;
    std::cout << "Number of colors: " << colors.size() << std::endl;
    std::cout << "Colors: ";
    for (int c : colors) std::cout << c << " ";
    std::cout << std::endl;

    // For a valid solution, we need exactly n queens (one per row, column, color)
    if (colors.size() != n) {
        std::cout << "WARNING: Number of colors != board size" << std::endl;
    }

    return true;
}

int main() {
    // Test first puzzle
    std::vector<std::vector<int>> puzzle1 = {
        {1, 1, 2, 2, 2, 3, 3, 3},
        {1, 4, 2, 4, 2, 5, 3, 3},
        {1, 4, 2, 4, 2, 3, 3, 3},
        {1, 4, 4, 4, 2, 6, 7, 3},
        {1, 4, 4, 4, 2, 6, 7, 7},
        {1, 4, 8, 4, 2, 6, 7, 7},
        {8, 4, 8, 4, 2, 6, 6, 7},
        {8, 8, 8, 8, 7, 7, 7, 7}
    };

    std::cout << "=== Puzzle 1 ===" << std::endl;
    isValidSolution(puzzle1);

    // Check puzzle 6 which solved
    std::vector<std::vector<int>> puzzle6 = {
        {1, 1, 2, 2, 2, 2, 3, 3},
        {1, 2, 2, 4, 4, 2, 2, 3},
        {1, 2, 4, 4, 4, 4, 2, 3},
        {5, 2, 4, 4, 6, 4, 2, 3},
        {5, 2, 2, 4, 6, 6, 2, 3},
        {5, 5, 2, 2, 2, 6, 6, 3},
        {5, 5, 5, 5, 5, 7, 6, 8},
        {5, 5, 5, 7, 7, 7, 8, 8}
    };

    std::cout << "\n=== Puzzle 6 (SOLVED) ===" << std::endl;
    isValidSolution(puzzle6);

    return 0;
}
