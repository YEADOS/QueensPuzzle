#include "include/PuzzleManager.h"
#include "include/PuzzleSolver.h"
#include <iostream>

// Test to verify TRUE active sensing behavior
int main() {
    // Load just one puzzle for detailed analysis
    std::vector<Graph> graphs;
    PuzzleManager::loadFromFile("puzzles.txt", 1, graphs);

    if (graphs.empty()) {
        std::cout << "No puzzles loaded" << std::endl;
        return 1;
    }

    std::cout << "========================================" << std::endl;
    std::cout << "TESTING TRUE ACTIVE SENSING" << std::endl;
    std::cout << "========================================" << std::endl;

    Graph& g = graphs[0];
    std::cout << "\nOriginal puzzle (ground truth):" << std::endl;
    g.printGraph(g.ORIGINAL);

    std::cout << "\nInitial masked state (70% hidden):" << std::endl;
    g.printGraph(g.MASKED);

    PuzzleSolver solver(g);

    std::cout << "\n========================================" << std::endl;
    std::cout << "STARTING SOLVER - Watch for strategic probing!" << std::endl;
    std::cout << "========================================\n" << std::endl;

    bool solved = solver.solveWithMinimalProbing(g.getSize());

    std::cout << "\n========================================" << std::endl;
    std::cout << "SOLVER COMPLETED" << std::endl;
    std::cout << "========================================" << std::endl;

    if (solved) {
        std::cout << "\n✅ Solution found!" << std::endl;
        std::cout << "\nFinal board state:" << std::endl;
        g.printGraph(g.CURRENT_SYMBOLS);
        solver.verifyQueenPlacement();
    } else {
        std::cout << "\n❌ No solution found" << std::endl;
        std::cout << "\nBest partial solution:" << std::endl;
        g.printGraph(g.CURRENT_SYMBOLS);
    }

    solver.printStatistics();

    std::cout << "\n========================================" << std::endl;
    std::cout << "ANALYSIS OF ACTIVE SENSING BEHAVIOR" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Inference ratio: " << (double)solver.inferredCount / (solver.inferredCount + solver.probeCount) << std::endl;
    std::cout << "Probes per queen: " << (double)solver.probeCount / g.getSize() << std::endl;
    std::cout << "If this is < 1.0, we're using strategic probing!" << std::endl;

    return 0;
}
