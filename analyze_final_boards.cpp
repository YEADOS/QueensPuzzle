#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include "include/PuzzleManager.h"
#include "include/PuzzleSolver.h"
#include "include/graph.h"

int main() {
    std::string filename = "puzzles.txt";
    std::vector<Graph> puzzles;
    PuzzleManager::loadFromFile(filename, 20, puzzles);

    std::cout << "=== ANALYSIS: How much of each board is revealed? ===\n\n";

    int totalPuzzles = 0;
    int solvedPuzzles = 0;
    int totalCells = 0;
    int totalUnknown = 0;

    for (size_t idx = 0; idx < puzzles.size() && idx < 20; idx++) {
        Graph puzzle = puzzles[idx];
        int n = puzzle.getSize();

        // Count initially unknown (masked is already created by loadFromFile)
        int initialUnknown = 0;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (puzzle.getMasked()[i][j] == -1) {
                    initialUnknown++;
                }
            }
        }

        // Solve
        PuzzleSolver solver(puzzle);
        bool solved = solver.solveWithActiveCSP(n);

        // Count final unknown
        int finalUnknown = 0;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (puzzle.getMasked()[i][j] == -1) {
                    finalUnknown++;
                }
            }
        }

        int boardSize = n * n;
        int revealed = initialUnknown - finalUnknown;
        double revealPercent = (double)revealed / initialUnknown * 100.0;
        double finalUnknownPercent = (double)finalUnknown / boardSize * 100.0;

        totalPuzzles++;
        if (solved) solvedPuzzles++;
        totalCells += boardSize;
        totalUnknown += finalUnknown;

        std::cout << "Puzzle " << (idx + 1) << " (" << n << "x" << n << " = " << boardSize << " cells):\n";
        std::cout << "  Initially unknown: " << initialUnknown << " / " << boardSize
                  << " (" << (double)initialUnknown/boardSize*100 << "%)\n";
        std::cout << "  Cells revealed during solving: " << revealed << " / " << initialUnknown
                  << " (" << revealPercent << "%)\n";
        std::cout << "  Final unknown: " << finalUnknown << " / " << boardSize
                  << " (" << finalUnknownPercent << "%)\n";
        std::cout << "  Result: " << (solved ? "✅ SOLVED" : "❌ FAILED") << "\n";
        std::cout << "  Probes: " << solver.probeCount
                  << ", Inferences: " << solver.inferredCount << "\n\n";
    }

    std::cout << "=== AGGREGATE STATISTICS ===\n";
    std::cout << "Success rate: " << solvedPuzzles << "/" << totalPuzzles
              << " (" << (double)solvedPuzzles/totalPuzzles*100 << "%)\n";
    std::cout << "Total cells across all puzzles: " << totalCells << "\n";
    std::cout << "Total unknown cells remaining: " << totalUnknown << "\n";
    std::cout << "Average unknowns per puzzle: " << (double)totalUnknown/totalPuzzles << "\n";
    std::cout << "Overall board revelation: " << (double)(totalCells - totalUnknown)/totalCells*100 << "%\n";

    return 0;
}
