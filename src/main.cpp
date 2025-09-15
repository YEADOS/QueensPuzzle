#include "../include/PuzzleManager.h"
#include "../include/PuzzleSolver.h"
#include <string>

int main()
{

    int numPuzzles = 1;
    std::string puzzleFileName = "puzzles.txt";
    std::vector<Graph> graphs;
    // std::vector<Graph> Graphs; = PuzzleManager::loadFromFile(puzzleFileName, numPuzzles);
    PuzzleManager::loadFromFile(puzzleFileName, numPuzzles, graphs);
    for (auto &g : graphs)
    {

        PuzzleSolver solver(g);
        // g.printGraph(g.ORIGINAL);
        // Initial masked
        g.printGraph(g.MASKED);

        if (!solver.solvePuzzle(0, g.getSize()))
        {
            std::cout << "No solution found.\n";
        }
        else
        {
            std::cout << "Original.\n";
            g.printGraph(g.ORIGINAL);
            std::cout << "Current.\n";
            g.printGraph(g.CURRENT_SYMBOLS);
            std::cout << "Finished Masked.\n";
            g.printGraph(g.MASKED);
            std::cout << "Solution found.\n";
        }
        solver.printStatistics();

        // if (!g.solvePuzzle(0, g.getSize())) {
        //     std::cout << "No solution found.\n";
        // }
    }

    return 0;
}
