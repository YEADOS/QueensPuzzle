#include "../include/PuzzleManager.h"
#include <fstream>
#include <iostream>

std::vector<Graph> PuzzleManager::loadFromFile(const std::string& filename, int numPuzzles) {
    std::vector<Graph> graphs;
    std::ifstream puzzleFile(filename);

    if (!puzzleFile.is_open()) {
        std::cerr << "Unable to open file: " << filename << std::endl;
        return graphs;
    }

    for (int i = 0; i < numPuzzles; i++) {
        int graphSize;
        puzzleFile >> graphSize;

        // Create 2d Matrix to hold txt data - resize to graphSize x graphSize
        std::vector<std::vector<int>> puzzleData(graphSize, std::vector<int>(graphSize));

        for (int row = 0; row < graphSize; row++) {
            for (int col = 0; col < graphSize; col++) {
                puzzleFile >> puzzleData[row][col];
            }
        }
        Graph newGraph(puzzleData);
        graphs.push_back(newGraph);
    }
    puzzleFile.close();
    return graphs;
}