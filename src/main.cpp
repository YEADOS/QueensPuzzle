#include "../include/PuzzleManager.h"
#include "../include/PuzzleSolver.h"
#include <string>

// Key: {Queen = 0, Masked = -1, Colour Square = 1 to N-Colours}

int main()
{

    int numPuzzles = 20;
    std::string puzzleFileName = "puzzles.txt";
    std::vector<Graph> graphs;
    // std::vector<Graph> Graphs; = PuzzleManager::loadFromFile(puzzleFileName, numPuzzles);
    PuzzleManager::loadFromFile(puzzleFileName, numPuzzles, graphs);

    int solvedCount = 0;
    int puzzleNumber = 1;

    for (auto &g : graphs)
    {
        std::cout << "\n===== PUZZLE " << puzzleNumber << "/" << numPuzzles << " =====" << std::endl;

        PuzzleSolver solver(g);
        // g.printGraph(g.ORIGINAL);
        // Initial masked
        g.printGraph(g.MASKED);

        if (!solver.solveWithMinimalProbing(g.getSize()))
        {
            std::cout << "❌ PUZZLE " << puzzleNumber << " - No solution found.\n";
        }
        else
        {
            std::cout << "✅ PUZZLE " << puzzleNumber << " - Solution found!\n";
            solvedCount++;

            std::cout << "Original:\n";
            g.printGraph(g.ORIGINAL);
            std::cout << "Current:\n";
            g.printGraph(g.CURRENT_SYMBOLS);
            std::cout << "Final Masked:\n";
            g.printGraph(g.MASKED);

            solver.verifyQueenPlacement();
        }
        solver.printStatistics();

        puzzleNumber++;

        // if (!g.solvePuzzle(0, g.getSize())) {
        //     std::cout << "No solution found.\n";
        // }
    }

    std::cout << "\n======= FINAL RESULTS =======" << std::endl;
    std::cout << "Solved: " << solvedCount << "/" << numPuzzles << " puzzles" << std::endl;
    std::cout << "Success rate: " << (100.0 * solvedCount / numPuzzles) << "%" << std::endl;

    return 0;
}
