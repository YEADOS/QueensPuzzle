#pragma once
#include "graph.h"

class RoboticSolver {
    public:
        RoboticSolver(Graph& graph, double mask_prob = 0.5);
        bool solve();
        void printSolution();
        void printMaskedMatrix();
    private:
        Graph& boardGraph;
        std::vector<std::vector<int>> originalPuzzle;
        std::vector<std::vector<int>> colourMask;
        std::vector<std::vector<int>> queenBoard;
        int probe(int row, int col);
        int nextRowHeuristic();
        bool isSafe(int row, int col);
        bool solveRecursive(int row);
        static std::vector<std::vector<int>> createMaskedMatrix(const std::vector<std::vector<int>>& original, double mask_prob);
};