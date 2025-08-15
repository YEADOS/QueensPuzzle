#include "../include/PuzzleManager.h"
#include <string>

int main() {

    int numPuzzles = 10;
    std::string puzzleFileName = "puzzles.txt";

    std::vector<Graph> Graphs = PuzzleManager::loadFromFile(puzzleFileName, numPuzzles);
    for (auto &g : Graphs) {
        if (!g.solvePuzzle(0, g.getSize())) {
            std::cout << "No solution found.\n";
        }
    }

    return 0;
}