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
    int totalQueensPlaced = 0;  // Track total queens placed (not decremented on backtrack)

    // MINIMAL SENSING: Probe budget system
    int probeBudget = 0;          // Maximum probes allowed
    int initialUnknownCells = 0;   // Count of unknown cells at start
    bool budgetExhausted = false;  // Flag when budget is depleted

    // Track best partial solution for failed puzzles
    std::vector<std::pair<int, int>> bestPartialSolution;
    int maxQueensPlaced = 0;

    PuzzleSolver(Graph &graph);

    int inferNeighbours(int row, int col);
    void probe(int row, int col);
    bool isValid(int row, int col);
    bool solveWithMinimalProbing(int n);
    bool solveRowWithBacktracking(int row, int n, std::vector<std::pair<int, int>>& queenPositions);
    std::vector<std::pair<int, int>> findViableQueenPositions(int row, int n);
    bool needsProbeForDecision(int row, int col);
    void undoQueenPlacement(int row, int col);
    void printStatistics();
    void verifyQueenPlacement();

    // Active sensing methods
    double calculateProbeValue(int row, int col, int n);
    void performInferenceCascade(int n);
    int countUnknownNeighbors(int row, int col, int n);
    bool hasQueenInColor(int color);
    bool validateFinalSolution(std::vector<std::pair<int, int>>& queenPositions);
    void restoreBestPartialSolution();
    void strategicSeedProbing(int n);

    // TRUE ACTIVE SENSING - Core methods
    bool solveWithActiveCSP(int n);  // New CSP solver with active sensing
    bool solveActiveCSPBacktrack(int row, int n, std::vector<std::pair<int, int>>& queenPositions);
    std::vector<std::pair<int, int>> getMostInformativeProbes(int k, std::vector<std::pair<int, int>>& viablePositions);
    double calculateExpectedInformationGain(int row, int col, int n);
    void propagateConstraints(int n);
    std::pair<int, int> selectMostConstrainedVariable(std::set<std::pair<int, int>>& unassigned);
    std::vector<std::pair<int, int>> getViablePositionsForCell(int row, int col, int n);
    int countViablePositions(int row, int col, int n);

    // MINIMAL SENSING - Budget management
    void initializeProbeBudget(int n, double budgetPercent = 0.15);  // Set budget as % of unknown cells
    bool canProbe();  // Check if budget allows probing
    int getMostLikelyColor(int row, int col, double& confidence);  // Get color with confidence score
    bool tryPlaceWithConfidence(int row, int col, double minConfidence);  // Try placement with incomplete info
};

#endif
