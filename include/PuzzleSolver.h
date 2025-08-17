#ifndef PUZZLE_SOLVER_H
#define PUZZLE_SOLVER_H 

#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <fstream>
#include "graph.h"

class PuzzleSolver {

    private: 

        Graph &puzzle;

    public:

        int queensPlaced;
        int backtrackCount;
        int probeCount;

        PuzzleSolver(Graph &graph);

        void processColours();
        void probe(int row, int col);
        bool isValid(int row, int col);
        bool solvePuzzle(int start, int size);
        void printStatistics();

};

#endif

