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

        PuzzleSolver(Graph &graph);

        void processColours();
        bool isValid(int row, int col);
        bool solvePuzzle(int start, int size);

};

#endif

