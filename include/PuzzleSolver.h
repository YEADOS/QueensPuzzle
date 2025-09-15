#ifndef PUZZLE_SOLVER_H
#define PUZZLE_SOLVER_H

#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <fstream>
#include "graph.h"
#include <set>
#include <cfloat>
#include <climits>
#include <map>

struct ColorDomain
{
    int minRow = INT_MAX, maxRow = -1;
    int minCol = INT_MAX, maxCol = -1;

    bool contains(int row, int col)
    {
        return row >= minRow && row <= maxRow &&
               col >= minCol && col <= maxCol;
    }
};

class PuzzleSolver
{

private:
    Graph &puzzle;
    void computeColorDomains(std::map<int, ColorDomain> &domains);
    int inferWithConfidence(int row, int col);
    int inferFromContiguity(int row, int col);
    int inferFromDomains(int row, int col);
    int inferRowColumnUniformity(int row, int col);
    int inferPatternCompletion(int row, int col);

public:
    static const int directions[4][2]; // Directions for checking diagonals and columns

    int queensPlaced = 0;
    int backtrackCount = 0;
    int probeCount = 0;
    int inferredCount = 0;

    PuzzleSolver(Graph &graph);

    void performFullInference(int n);
    int inferNeighbours(int row, int col);
    void processColours();
    void probe(int row, int col);
    bool isValid(int row, int col);
    bool solvePuzzle(int start, int size);
    void printStatistics();
};

#endif
