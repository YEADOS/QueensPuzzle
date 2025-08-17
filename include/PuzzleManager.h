#pragma once
#include "graph.h"
#include <vector>
#include <string>

class PuzzleManager {
public:
    // static std::vector<Graph> loadFromFile(const std::string& filename, int numPuzzles);
    static void loadFromFile(const std::string& filename, int numPuzzles, std::vector<Graph> &graphs);
};