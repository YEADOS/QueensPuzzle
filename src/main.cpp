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
    auto solutionsPos = PuzzleSolver::loadSolutions("solutions.txt");
    auto solutionBoards = PuzzleSolver::loadVisualSolutions("solutions_board.txt");

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
            
            if (solutionBoards.find(puzzleNumber) != solutionBoards.end()) {
                std::cout << "Correct Solution Board:\n";
                for (const auto& row : solutionBoards[puzzleNumber]) {
                    std::cout << row << std::endl;
                }
            }
        }
        // when the solver does not fail
        else
        {
            std::cout << "✅ PUZZLE " << puzzleNumber << " - Solution found!\n";
            std::cout << "Current:\n";
            g.printGraph(g.CURRENT_SYMBOLS);
            std::cout << "Final Masked:\n";
            g.printGraph(g.MASKED);

            solvedCount++;
        }

        solver.printStatistics();

        // Compare with correct solution (for both solved and failed)
        if (solutionsPos.find(puzzleNumber) != solutionsPos.end()) {
            auto& correctPositions = solutionsPos[puzzleNumber];
            solver.printCorrectnessReport(puzzleNumber, correctPositions);

            double correctness = solver.compareToCorrectPositions(puzzleNumber, correctPositions);
            totalCorrectness += correctness;
            totalPuzzles++;

            std::cout << "Correctness score: " << (correctness * 100.0) << "%" << std::endl;
        }

        puzzleNumber++;
    }

    std::cout << "\n[ FINAL RESULTS ]\n";
    std::cout << "Solved: " << solvedCount << "/" << numPuzzles << " puzzles\n";
    std::cout << "Success rate: " << (100.0 * solvedCount / numPuzzles) << "%\n";

    if (totalPuzzles > 0) {
        double avgCorrectness = totalCorrectness / totalPuzzles * 100.0;
        std::cout << "\n[ CORRECTNESS ANALYSIS ]\n";
        std::cout << "Average correctness: " << avgCorrectness << "%\n";
        std::cout << "Total correct placements: " << (int)(totalCorrectness) << " / " << totalPuzzles << " puzzles\n";
        std::cout << "(Includes both successful and failed attempts)\n";
    }

    // Explicitly clear the graphs vector to avoid cleanup issues with large datasets
    graphs.clear();

    return 0;
}
