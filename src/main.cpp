#include "../include/PuzzleManager.h"
#include "../include/PuzzleSolver.h"
#include <string>

// Key: {Queen = 0, Masked = -1, Colour Square = 1 to N-Colours}

int main()
{

    int numPuzzles = 2;
    std::string puzzleFileName = "puzzles.txt";
    std::vector<Graph> graphs;
    // std::vector<Graph> Graphs; = PuzzleManager::loadFromFile(puzzleFileName, numPuzzles);
    PuzzleManager::loadFromFile(puzzleFileName, numPuzzles, graphs);

    // Load solutions to verify PuzzleSolver results 
    auto solutions = PuzzleSolver::loadSolutions("solutions.txt");
    auto visualSolutions = PuzzleSolver::loadVisualSolutions("solutions_board.txt");

    int solvedCount = 0;
    int puzzleNumber = 1;

    // Track correctness statistics
    double totalCorrectness = 0.0;
    int totalPuzzles = 0;

    for (auto &g : graphs)
    {
        std::cout << "\n------ PUZZLE " << puzzleNumber << "/" << numPuzzles << " ------\n\n";

        PuzzleSolver solver(g);
        // g.printGraph(g.ORIGINAL);
        // Initial masked before board is altered (starting state of problem)
        std::cout << "Original Masked:\n";
        g.printGraph(g.MASKED);
        // Full board with colours revealed
        std::cout << "Original:\n";
        g.printGraph(g.ORIGINAL);

        bool solved = solver.solvePuzzle(g.getSize());  // Using minimal sensing solver

        // when the solver fails
        if (!solved)
        {
            std::cout << "\n❌ PUZZLE " << puzzleNumber << " - No solution found.\n";
            std::cout << "Current State of failed board (queens placed so far):\n";
            g.printGraph(g.CURRENT_SYMBOLS);
            
            if (visualSolutions.find(puzzleNumber) != visualSolutions.end()) {
                std::cout << "Correct Solution Board:\n";
                for (const auto& row : visualSolutions[puzzleNumber]) {
                    std::cout << row << std::endl;
                }
            }
        }
        // when the solver does not fail
        else
        {
            std::cout << "✅ PUZZLE " << puzzleNumber << " - Solution found!\n";
            solvedCount++;


            std::cout << "Current:\n";
            g.printGraph(g.CURRENT_SYMBOLS);
            std::cout << "Final Masked:\n";
            g.printGraph(g.MASKED);

            solver.verifyQueenPlacement();
        }
        solver.printStatistics();

        // Compare with ground truth (for both solved and failed)
        if (solutions.find(puzzleNumber) != solutions.end()) {
            auto& groundTruth = solutions[puzzleNumber];
            solver.printCorrectnessReport(puzzleNumber, groundTruth);

            double correctness = solver.compareWithGroundTruth(puzzleNumber, groundTruth);
            totalCorrectness += correctness;
            totalPuzzles++;

            std::cout << "Correctness score: " << (correctness * 100.0) << "%" << std::endl;
        }

        puzzleNumber++;

        // if (!g.solvePuzzle(0, g.getSize())) {
        //     std::cout << "No solution found.\n";
        // }
    }

    std::cout << "\n======= FINAL RESULTS =======" << std::endl;
    std::cout << "Solved: " << solvedCount << "/" << numPuzzles << " puzzles" << std::endl;
    std::cout << "Success rate: " << (100.0 * solvedCount / numPuzzles) << "%" << std::endl;

    if (totalPuzzles > 0) {
        double avgCorrectness = totalCorrectness / totalPuzzles * 100.0;
        std::cout << "\n======= CORRECTNESS ANALYSIS =======" << std::endl;
        std::cout << "Average correctness: " << avgCorrectness << "%" << std::endl;
        std::cout << "Total correct placements: " << (int)(totalCorrectness) << " / " << totalPuzzles << " puzzles" << std::endl;
        std::cout << "(Includes both successful and failed attempts)" << std::endl;
    }

    // Explicitly clear the graphs vector to avoid cleanup issues with large datasets
    graphs.clear();

    return 0;
}
