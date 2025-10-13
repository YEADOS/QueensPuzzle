#ifndef GRAPH_H
#define GRAPH_H 

#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <fstream>
#include <random>
#include <iostream>


class Graph {

    public: 
        enum PrintMode {
            ORIGINAL,
            MASKED,
            CURRENT_RAW,      // Print current state as numbers
            CURRENT_SYMBOLS   // Print current state as Q/. symbols
        };

    private:
        const std::vector<std::vector<int>> original;
        std::vector<std::vector<int>> masked;
        std::vector<std::vector<int>> currentState;

        std::vector<std::vector<int>> createMaskedMatrix(const std::vector<std::vector<int>>& original, double mask_prob);
        std::vector<std::vector<int>> createMaskedMatrix(std::string filename);
        std::vector<std::vector<int>> createSmartMaskedMatrix(const std::vector<std::vector<int>>& original, double mask_prob);
        
    public: 

        Graph();
        Graph(const std::vector<std::vector<int>>& data);

        // const void printGraph();    
        void printGraph(PrintMode mode = ORIGINAL) const;
        const std::vector<std::vector<int>> &getOriginal();
        std::vector<std::vector<int>> &getMasked();
        std::vector<std::vector<int>> &getCurrentState();
        int getSize() const;
};

#endif