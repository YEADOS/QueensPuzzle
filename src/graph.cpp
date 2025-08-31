#include "../include/graph.h"

// Queen is represented by 0
// Non-queen spot represented by -1

// Default Constructor
Graph::Graph() {
    // puzzle = {
    //     {1, 2, 2, 2, 2, 2, 2, 2, 2},
    //     {1, 1, 2, 3, 3, 3, 2, 2, 2},
    //     {1, 2, 2, 3, 4, 3, 2, 2, 2},
    //     {1, 7, 2, 4, 4, 4, 5, 5, 2},
    //     {1, 7, 4, 4, 4, 4, 4, 5, 2},
    //     {1, 7, 7, 6, 4, 6, 5, 5, 2},
    //     {1, 1, 1, 6, 4, 6, 2, 2, 2},
    //     {9, 8, 8, 6, 6, 6, 2, 2, 2},
    //     {9, 9, 8, 8, 8, 8, 8, 2, 2}
    // };
    // puzzle = {
    //     {1, 1, 1, 1, 1, 1, 1, 1},
    //     {1, 1, 2, 2, 2, 3, 1, 1},
    //     {1, 1, 2, 3, 2, 3, 1, 1},
    //     {4, 4, 3, 3, 3, 3, 5, 5},
    //     {4, 6, 6, 3, 3, 7, 7, 5},
    //     {4, 4, 6, 3, 3, 3, 7, 5},
    //     {8, 6, 6, 8, 3, 7, 7, 5},
    //     {8, 8, 8, 8, 3, 3, 5, 5}
    // };

    // 30/4/2025 LinkedIn Queens Game

    // puzzle = {
    //     {1, 2, 2, 2, 2, 2, 2},
    //     {1, 1, 1, 1, 3, 3, 2},
    //     {1, 1, 4, 5, 5, 3, 2},
    //     {1, 6, 4, 4, 5, 2, 2},
    //     {1, 6, 6, 4, 4, 2, 2},
    //     {7, 7, 6, 6, 2, 2, 2},
    //     {7, 7, 2, 2, 2, 2, 2}
    // };
    // puzzleBackup = puzzle;
}

// Constructor
Graph::Graph(const std::vector<std::vector<int>>& data) 
    : original(data), currentState(data), masked(createMaskedMatrix("puzzles_masked.txt")) {}

const std::vector<std::vector<int>>& Graph::getOriginal() {
    return original;
}

std::vector<std::vector<int>>& Graph::getMasked() {
    return masked;
}

std::vector<std::vector<int>>& Graph::getCurrentState() {
    return currentState;
}


int Graph::getSize() const {
    return original.size();
}

void Graph::printGraph(PrintMode mode) const {

    const std::vector<std::vector<int>>* puzzleType = nullptr;
    
    switch (mode) {
        case ORIGINAL:
            puzzleType = &original;
            break;
        case MASKED:
            puzzleType = &masked;
            break;
        case CURRENT_RAW:
        case CURRENT_SYMBOLS:
            puzzleType = &currentState;
            break;
    }
    
    if (!puzzleType || puzzleType->empty()) {
        std::cout << "Empty matrix" << std::endl;
        return;
    }
    
    int n = getSize();
    
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            int value = (*puzzleType)[i][j];
            
            if (mode == CURRENT_SYMBOLS) {
                if (value == 0)
                    std::cout << "Q ";
                else
                    std::cout << ". ";
            } else {
                std::cout << value << " ";
            }
        }
        std::cout << '\n';
    }
    std::cout << "---------------------\n";
}

// const void Graph::printGraph() {
//     int n = getSize();
//     for (int i = 0; i < n; ++i) {
//         for (int j = 0; j < n; ++j) {
//             std::cout << original[i][j] << " ";
//         }
//         std::cout << '\n';
//     }
//     std::cout << "---------------------\n";
// }

std::vector<std::vector<int>> Graph::createMaskedMatrix(const std::vector<std::vector<int>>& original, double mask_prob) {
    std::vector<std::vector<int>> masked = original;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::bernoulli_distribution mask(mask_prob);

    for (size_t i = 0; i < masked.size(); ++i) {
        for (size_t j = 0; j < masked[i].size(); ++j) {
            if (mask(gen)) {
                masked[i][j] = -1;
            }
        }
    }
    return masked;
}

std::vector<std::vector<int>> Graph::createMaskedMatrix(std::string filename) {
    std::vector<std::vector<int>> masked;
    std::ifstream puzzleFile(filename);

    if (!puzzleFile.is_open()) {
        std::cerr << "Unable to open file: " << filename << std::endl;
        return masked;
    }

    int graphSize;
    puzzleFile >> graphSize;

    // Create 2d Matrix to hold txt data - resize to graphSize x graphSize
    masked.resize(graphSize, std::vector<int>(graphSize));

    for (int row = 0; row < graphSize; row++) {
        for (int col = 0; col < graphSize; col++) {
            puzzleFile >> masked[row][col];
        }
    }
    puzzleFile.close();
    return masked;
}
