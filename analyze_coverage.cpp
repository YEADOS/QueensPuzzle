#include "include/PuzzleManager.h"
#include "include/PuzzleSolver.h"
#include <iostream>

int countUnknown(const std::vector<std::vector<int>>& board) {
    int count = 0;
    for (const auto& row : board) {
        for (int val : row) {
            if (val == -1) count++;
        }
    }
    return count;
}

int main() {
    std::vector<Graph> graphs;
    PuzzleManager::loadFromFile("puzzles.txt", 5, graphs);

    for (size_t i = 0; i < graphs.size(); i++) {
        Graph& g = graphs[i];
        int n = g.getSize();
        int totalCells = n * n;

        std::cout << "\n=== PUZZLE " << (i+1) << " (size " << n << "x" << n << " = " << totalCells << " cells) ===" << std::endl;

        int initialUnknown = countUnknown(g.getMasked());
        std::cout << "Initially unknown: " << initialUnknown << " / " << totalCells
                  << " (" << (100.0 * initialUnknown / totalCells) << "%)" << std::endl;

        PuzzleSolver solver(g);
        bool solved = solver.solveWithMinimalProbing(n);

        int finalUnknown = countUnknown(g.getMasked());
        std::cout << "Finally unknown: " << finalUnknown << " / " << totalCells
                  << " (" << (100.0 * finalUnknown / totalCells) << "%)" << std::endl;

        std::cout << "Probes: " << solver.probeCount << std::endl;
        std::cout << "Inferences: " << solver.inferredCount << std::endl;
        int revealed = initialUnknown - finalUnknown;
        std::cout << "Total revealed: " << revealed << " (probes + inferences)" << std::endl;

        if (revealed != solver.probeCount + solver.inferredCount) {
            std::cout << "⚠️  WARNING: Math doesn't add up!" << std::endl;
            std::cout << "   Revealed: " << revealed << std::endl;
            std::cout << "   Probes + Inferences: " << (solver.probeCount + solver.inferredCount) << std::endl;
            std::cout << "   Difference: " << (revealed - solver.probeCount - solver.inferredCount) << std::endl;
        }

        std::cout << "Solved: " << (solved ? "YES" : "NO") << std::endl;
    }

    return 0;
}
