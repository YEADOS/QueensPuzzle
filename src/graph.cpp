#include "../include/graph.h"
#include <map>
#include <set>

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
    : original(data), currentState(data), masked(createMaskedMatrix(data, 0.3)) {}

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
    std::cout << "---------------------\n";
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
    std::cout << "---------------------\n\n";
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

std::vector<std::vector<int>> Graph::createSmartMaskedMatrix(const std::vector<std::vector<int>>& original, double mask_prob) {
    std::vector<std::vector<int>> masked = original;
    int n = original.size();

    // Step 1: Count cells for each color
    std::map<int, int> colourCounts;
    std::map<int, std::vector<std::pair<int, int>>> colourPositions;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int colour = original[i][j];
            colourCounts[colour]++;
            colourPositions[colour].push_back({i, j});
        }
    }

    // Step 2: Identify single-cell colored sections (don't mask these)
    std::set<std::pair<int, int>> preservePositions;
    for (auto& [colour, count] : colourCounts) {
        if (count == 1) {
            // Preserve single-cell colours
            preservePositions.insert(colourPositions[colour][0]);
            std::cout << "Preserving single-cell colour " << colour
                     << " at (" << colourPositions[colour][0].first
                     << "," << colourPositions[colour][0].second << ")\n";
        }
    }

    // Step 3: Smart masking strategy
    std::random_device rd;
    std::mt19937 gen(rd());
    std::bernoulli_distribution mask(mask_prob);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            // Never mask single-cell colours
            if (preservePositions.count({i, j})) {
                continue;
            }

            int colour = original[i][j];

            // For colours with few cells (2-3), mask more conservatively
            if (colourCounts[colour] <= 3) {
                std::bernoulli_distribution conservativeMask(mask_prob * 0.5);
                if (conservativeMask(gen)) {
                    masked[i][j] = -1;
                }
            }
            // For edge/corner positions, mask less aggressively (easier to infer)
            else if (i == 0 || i == n-1 || j == 0 || j == n-1) {
                std::bernoulli_distribution edgeMask(mask_prob * 0.7);
                if (edgeMask(gen)) {
                    masked[i][j] = -1;
                }
            }
            // Regular masking for other positions
            else if (mask(gen)) {
                masked[i][j] = -1;
            }
        }
    }

    // Step 4: Ensure each colour has at least one visible cell for inference
    for (auto& [colour, positions] : colourPositions) {
        bool hasVisible = false;
        for (auto& pos : positions) {
            if (masked[pos.first][pos.second] != -1) {
                hasVisible = true;
                break;
            }
        }

        // If all cells of a colour are masked, reveal at least one
        if (!hasVisible && !positions.empty()) {
            int revealIdx = gen() % positions.size();
            auto revealPos = positions[revealIdx];
            masked[revealPos.first][revealPos.second] = colour;
            std::cout << "Revealing at least one cell of colour " << colour
                     << " at (" << revealPos.first << "," << revealPos.second << ")\n";
        }
    }

    return masked;
}
