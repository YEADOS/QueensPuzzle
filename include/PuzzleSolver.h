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

struct ColourDomain
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
    void computeColourDomains(std::map<int, ColourDomain> &domains);
    int inferStrict(int row, int col);
    int inferFromContiguity(int row, int col);
    int inferFromDomains(int row, int col);
    int inferRowColumnUniformity(int row, int col);
    int inferPatternCompletion(int row, int col);

public:
    static const int directions[4][2];

    int queensPlaced = 0;
    int backtrackCount = 0;
    int probeCount = 0;
    int inferredCount = 0;
    int totalQueensPlaced = 0;

    int probeBudget = 0;
    int initialUnknownCells = 0;
    bool budgetExhausted = false;

    std::vector<std::pair<int, int>> bestPartialSolution;
    int maxQueensPlaced = 0;

    PuzzleSolver(Graph &graph);

    int inferNeighbours(int row, int col);
    void probe(int row, int col);
    bool isValid(int row, int col);
    std::vector<std::pair<int, int>> findViableQueenPositions(int row, int n);
    void undoQueenPlacement(int row, int col);
    void printStatistics();

    double calculateProbeValue(int row, int col, int n);
    void performInferenceCascade(int n);
    int countUnknownNeighbours(int row, int col, int n);
    bool hasQueenInColour(int colour);
    bool validateFinalSolution(std::vector<std::pair<int, int>>& queenPositions);
    void restoreBestPartialSolution();

    bool solvePuzzle(int n);
    bool mainSolver(int row, int n, std::vector<std::pair<int, int>>& queenPositions);
    std::vector<std::pair<int, int>> findBestProbeSpots(int k, std::vector<std::pair<int, int>>& viablePositions);
    double calculateExpectedInformationGain(int row, int col, int n);
    void propagateConstraints(int n);

    void setProbeBudget(int n, double budgetPercent = 0.15);
    bool canProbe();
    int inferWeak(int row, int col, double& confidence);

    static std::map<int, std::vector<std::pair<int, int>>> loadSolutions(const std::string& filename);
    static std::map<int, std::vector<std::string>> loadVisualSolutions(const std::string& filename);
    double compareToCorrectPositions(int puzzleNumber, const std::vector<std::pair<int, int>>& groundTruth);
    void printCorrectnessReport(int puzzleNumber, const std::vector<std::pair<int, int>>& groundTruth);
};

#endif
