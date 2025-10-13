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
    double calculateProbeValue(int row, int col);
    std::pair<int, int> selectBestProbeTarget(int n);
    std::pair<int, int> selectGlobalBestProbe(int currentRow, int currentCol, int n);
    bool shouldSkipCurrentPosition(int currentRow, int currentCol, int n);
    bool isProbeNecessaryForQueenPlacement(int row, int col);
    std::pair<int, int> selectBestLocalProbe(int currentRow, int currentCol, int n);
    int simulateProbeOutcome(int row, int col, int n);
    std::vector<std::pair<int, int>> getLocalProbeCandidate(int currentRow, int currentCol, int n);

public:
    static const int directions[4][2]; // Directions for checking diagonals and columns

    int queensPlaced = 0;
    int backtrackCount = 0;
    int probeCount = 0;
    int inferredCount = 0;
    int sensingBudget = 15;  // Limited probes allowed
    double skipThreshold = 1.5;  // Skip current position if alternative is 1.5x better

    PuzzleSolver(Graph &graph);

    void performFullInference(int n);
    int inferNeighbours(int row, int col);
    void processColours();
    void probe(int row, int col);
    bool isValid(int row, int col);
    bool solvePuzzle(int start, int size);
    bool solveWithMinimalProbing(int n);
    bool solveRowWithBacktracking(int row, int n, std::vector<std::pair<int, int>>& queenPositions);
    std::vector<std::pair<int, int>> findViableQueenPositions(int row, int n);
    bool needsProbeForDecision(int row, int col);
    void undoQueenPlacement(int row, int col);
    void printStatistics();
    void verifyQueenPlacement();
};

#endif
