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

        static const int directions[4][2]; // Directions for checking diagonals and columns

        int queensPlaced;
        int backtrackCount;
        int probeCount;
        int inferredCount;

        PuzzleSolver(Graph &graph);

        int inferNeighbours(int row, int col);
        
        void processColours();
        void probe(int row, int col);
        bool isValid(int row, int col);
        bool solvePuzzle(int start, int size);
        void printStatistics();

};

#endif

