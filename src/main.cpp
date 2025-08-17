#include "../include/PuzzleManager.h"
#include "../include/PuzzleSolver.h"
#include <string>


int main() {

    int numPuzzles = 10;
    std::string puzzleFileName = "puzzles.txt";

    std::vector<Graph> Graphs = PuzzleManager::loadFromFile(puzzleFileName, numPuzzles);
    for (auto &g : Graphs) {


        PuzzleSolver solver(g);

        if (!solver.solvePuzzle(0, g.getSize())) {
            std::cout << "No solution found.\n";
        }
        else {
            
            std::cout << "Solution found.\n";
        }


        // if (!g.solvePuzzle(0, g.getSize())) {
        //     std::cout << "No solution found.\n";
        // }
    }

    return 0;
}

