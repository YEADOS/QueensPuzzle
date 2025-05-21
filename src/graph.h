#ifndef GRAPH_H
#define GRAPHS_H 

#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>


class Graph {
    private:
        std::vector<std::vector<int>> puzzle;
        std::vector<std::vector<int>> puzzleBackup;

    public: 

        Graph();

        void processColours();
        bool isValid(int row, int col);
        bool solvePuzzle(int start, int end);
        void printGraph();    
        std::vector<std::vector<int>> getGraph();
        int getSize();


};

#endif