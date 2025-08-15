#ifndef GRAPH_H
#define GRAPHS_H 

#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <fstream>


class Graph {
    private:
        std::vector<std::vector<int>> puzzle;
        std::vector<std::vector<int>> puzzleBackup;

    public: 

        Graph();
        Graph(const std::vector<std::vector<int>>& data);

        void processColours();
        bool isValid(int row, int col);
        bool solvePuzzle(int start, int end);
        const void printGraph();    
        const std::vector<std::vector<int>> getGraph();
        const int getSize();
};

#endif