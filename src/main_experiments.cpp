#include "../include/PuzzleManager.h"
#include "../include/PuzzleSolver.h"
#include <string>
#include <fstream>
#include <iomanip>
#include <ctime>

// Key: {Queen = 0, Masked = -1, Colour Square = 1 to N-Colours}

// Structure to hold aggregate statistics
struct AggregateStatistics
{
    int totalPuzzles = 0;
    int solvedPuzzles = 0;
    int failedPuzzles = 0;

    // Success metrics
    double successRate = 0.0;
    double avgCorrectnessAll = 0.0;        // Average across all puzzles
    double avgCorrectnessSolved = 0.0;     // Average for solved puzzles only
    double avgCorrectnessFailed = 0.0;     // Average for failed puzzles only

    // Failed puzzle specific metrics
    int totalCorrectQueensInFailed = 0;    // Sum of correct queens in failed puzzles
    int totalExpectedQueensInFailed = 0;   // Sum of expected queens in failed puzzles
    double failedPuzzleCorrectness = 0.0;  // Correctness for failed puzzles only

    // Probe efficiency metrics
    int totalProbesUsed = 0;
    int totalProbeBudget = 0;
    double avgProbesUsed = 0.0;
    double avgProbeBudgetUtilization = 0.0;  // probesUsed / probeBudget

    // Inference metrics
    int totalInferences = 0;
    double avgInferences = 0.0;
    double probeInferenceRatio = 0.0;      // inferences / probes

    // Masking/revealing metrics
    int totalInitialMasked = 0;
    int totalRevealed = 0;
    double avgRevealPercentage = 0.0;      // revealed / initial_masked * 100

    // Backtracking metrics
    int totalBacktracks = 0;
    double avgBacktracks = 0.0;

    // Grid size info
    double avgGridSize = 0.0;
};

// Calculate aggregate statistics from individual puzzle stats
AggregateStatistics calculateAggregateStats(const std::vector<PuzzleStatistics>& allStats)
{
    AggregateStatistics agg;
    agg.totalPuzzles = allStats.size();

    double totalCorrectnessAll = 0.0;
    double totalCorrectnessSolved = 0.0;
    double totalCorrectnessFailed = 0.0;
    int solvedCount = 0;
    int failedCount = 0;

    for (const auto& stat : allStats) {
        // Count solved/failed
        if (stat.solved) {
            agg.solvedPuzzles++;
            solvedCount++;
            totalCorrectnessSolved += stat.correctnessScore;
        } else {
            agg.failedPuzzles++;
            failedCount++;
            totalCorrectnessFailed += stat.correctnessScore;

            // Track correct queens in failed puzzles
            agg.totalCorrectQueensInFailed += stat.correctQueens;
            agg.totalExpectedQueensInFailed += stat.expectedQueens;
        }

        totalCorrectnessAll += stat.correctnessScore;

        // Accumulate other metrics
        agg.totalProbesUsed += stat.probesUsed;
        agg.totalProbeBudget += stat.probeBudget;
        agg.totalInferences += stat.inferences;
        agg.totalInitialMasked += stat.initialMaskedCells;
        agg.totalRevealed += stat.cellsRevealed;
        agg.totalBacktracks += stat.backtracks;
        agg.avgGridSize += stat.gridSize;
    }

    // Calculate averages and ratios
    if (agg.totalPuzzles > 0) {
        agg.successRate = (double)agg.solvedPuzzles / agg.totalPuzzles * 100.0;
        agg.avgCorrectnessAll = totalCorrectnessAll / agg.totalPuzzles * 100.0;
        agg.avgProbesUsed = (double)agg.totalProbesUsed / agg.totalPuzzles;
        agg.avgInferences = (double)agg.totalInferences / agg.totalPuzzles;
        agg.avgBacktracks = (double)agg.totalBacktracks / agg.totalPuzzles;
        agg.avgGridSize = agg.avgGridSize / agg.totalPuzzles;

        if (agg.totalInitialMasked > 0) {
            agg.avgRevealPercentage = (double)agg.totalRevealed / agg.totalInitialMasked * 100.0;
        }

        if (agg.totalProbeBudget > 0) {
            agg.avgProbeBudgetUtilization = (double)agg.totalProbesUsed / agg.totalProbeBudget * 100.0;
        }

        if (agg.totalProbesUsed > 0) {
            agg.probeInferenceRatio = (double)agg.totalInferences / agg.totalProbesUsed;
        }
    }

    if (solvedCount > 0) {
        agg.avgCorrectnessSolved = totalCorrectnessSolved / solvedCount * 100.0;
    }

    if (failedCount > 0) {
        agg.avgCorrectnessFailed = totalCorrectnessFailed / failedCount * 100.0;
        if (agg.totalExpectedQueensInFailed > 0) {
            agg.failedPuzzleCorrectness = (double)agg.totalCorrectQueensInFailed / agg.totalExpectedQueensInFailed * 100.0;
        }
    }

    return agg;
}

// Write aggregate statistics to a text file (append mode)
void writeStatisticsToFile(const std::string& filename, const AggregateStatistics& stats,
                           const std::string& configDescription,
                           int numPuzzles, double maskingPercent, double probeBudgetPercent)
{
    // Open in append mode
    std::ofstream outFile(filename, std::ios::app);

    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open file " << filename << " for writing.\n";
        return;
    }

    // Get current timestamp
    time_t now = time(0);
    char* dt = ctime(&now);

    outFile << "\n\n";
    outFile << "################################################################################\n";
    outFile << "################################################################################\n";
    outFile << "##                                                                            ##\n";
    outFile << "##                     NEW EXPERIMENT RUN                                     ##\n";
    outFile << "##                                                                            ##\n";
    outFile << "################################################################################\n";
    outFile << "################################################################################\n\n";

    outFile << "================================================================================\n";
    outFile << "                    PUZZLE SOLVER EXPERIMENT RESULTS                            \n";
    outFile << "================================================================================\n\n";

    outFile << "Experiment Date: " << dt;
    outFile << "\n[ TEST CONFIGURATION ]\n";
    outFile << "Number of Puzzles: " << numPuzzles << "\n";
    outFile << "Masking Percentage: " << std::fixed << std::setprecision(1) << (maskingPercent * 100) << "%\n";
    outFile << "Probe Budget Percentage: " << (probeBudgetPercent * 100) << "%\n";
    outFile << "Configuration: " << configDescription << "\n";
    outFile << "Total Puzzles Tested: " << stats.totalPuzzles << "\n\n";

    outFile << "--------------------------------------------------------------------------------\n";
    outFile << "                         SUCCESS METRICS                                        \n";
    outFile << "--------------------------------------------------------------------------------\n\n";

    outFile << std::fixed << std::setprecision(2);
    outFile << "Puzzles Solved:                  " << stats.solvedPuzzles << " / " << stats.totalPuzzles << "\n";
    outFile << "Puzzles Failed:                  " << stats.failedPuzzles << " / " << stats.totalPuzzles << "\n";
    outFile << "Success Rate:                    " << stats.successRate << "%\n\n";

    outFile << "Average Correctness (All):       " << stats.avgCorrectnessAll << "%\n";
    outFile << "Average Correctness (Solved):    " << stats.avgCorrectnessSolved << "%\n";
    outFile << "Average Correctness (Failed):    " << stats.avgCorrectnessFailed << "%\n\n";

    outFile << "--------------------------------------------------------------------------------\n";
    outFile << "                    FAILED PUZZLE ANALYSIS                                      \n";
    outFile << "--------------------------------------------------------------------------------\n\n";

    outFile << "Total Failed Puzzles:            " << stats.failedPuzzles << "\n";
    outFile << "Correct Queens in Failed:        " << stats.totalCorrectQueensInFailed << " / "
            << stats.totalExpectedQueensInFailed << "\n";
    outFile << "Failed Puzzle Correctness:       " << stats.failedPuzzleCorrectness << "%\n";
    outFile << "  (Percentage of correctly placed queens across all failed puzzles)\n\n";

    outFile << "--------------------------------------------------------------------------------\n";
    outFile << "                      PROBE EFFICIENCY METRICS                                  \n";
    outFile << "--------------------------------------------------------------------------------\n\n";

    outFile << "Total Probes Used:               " << stats.totalProbesUsed << " / "
            << stats.totalProbeBudget << " (budget)\n";
    outFile << "Average Probes per Puzzle:       " << stats.avgProbesUsed << "\n";
    outFile << "Probe Budget Utilization:        " << stats.avgProbeBudgetUtilization << "%\n";
    outFile << "  (Percentage of allocated probe budget actually used)\n\n";

    outFile << "--------------------------------------------------------------------------------\n";
    outFile << "                    INFERENCE & SENSING METRICS                                 \n";
    outFile << "--------------------------------------------------------------------------------\n\n";

    outFile << "Total Inferences Made:           " << stats.totalInferences << "\n";
    outFile << "Average Inferences per Puzzle:   " << stats.avgInferences << "\n";
    outFile << "Probe-to-Inference Ratio:        1:" << stats.probeInferenceRatio << "\n";
    outFile << "  (For every 1 probe, " << stats.probeInferenceRatio << " cells were inferred)\n\n";

    outFile << "--------------------------------------------------------------------------------\n";
    outFile << "                     INFORMATION REVEALING METRICS                              \n";
    outFile << "--------------------------------------------------------------------------------\n\n";

    outFile << "Total Initially Masked Cells:    " << stats.totalInitialMasked << "\n";
    outFile << "Total Cells Revealed:            " << stats.totalRevealed << "\n";
    outFile << "  - Via Probes:                  " << stats.totalProbesUsed << " ("
            << (stats.totalRevealed > 0 ? (double)stats.totalProbesUsed / stats.totalRevealed * 100.0 : 0.0) << "%)\n";
    outFile << "  - Via Inference:               " << stats.totalInferences << " ("
            << (stats.totalRevealed > 0 ? (double)stats.totalInferences / stats.totalRevealed * 100.0 : 0.0) << "%)\n";
    outFile << "Reveal Percentage:               " << stats.avgRevealPercentage << "%\n";
    outFile << "  (Percentage of masked cells that were revealed)\n\n";

    outFile << "--------------------------------------------------------------------------------\n";
    outFile << "                      BACKTRACKING METRICS                                      \n";
    outFile << "--------------------------------------------------------------------------------\n\n";

    outFile << "Total Backtracks:                " << stats.totalBacktracks << "\n";
    outFile << "Average Backtracks per Puzzle:   " << stats.avgBacktracks << "\n\n";

    outFile << "--------------------------------------------------------------------------------\n";
    outFile << "                         GENERAL INFORMATION                                    \n";
    outFile << "--------------------------------------------------------------------------------\n\n";

    outFile << "Average Grid Size:               " << stats.avgGridSize << " x " << stats.avgGridSize << "\n\n";

    outFile << "================================================================================\n";
    outFile << "                              END OF REPORT                                     \n";
    outFile << "================================================================================\n";

    outFile.close();

    std::cout << "\n✓ Statistics written to: " << filename << "\n";
}

int main(int argc, char* argv[])
{
    // Configuration parameters (can be passed as command line args)
    int numPuzzles = 100;
    double maskingPercentage = 0.3;      // 30% by default
    double probeBudgetPercent = 0.5;     // 50% by default
    std::string puzzleFileName = "puzzles.txt";
    std::string solutionsFileName = "solutions.txt";
    std::string outputFileName = "all_experiments.txt";  // Append to same file
    std::string configDescription = "";

    // Allow command line arguments for customization
    // Usage: ./experiments.out [numPuzzles] [maskingPercent] [probeBudgetPercent] [outputFile]
    if (argc >= 2) {
        numPuzzles = std::stoi(argv[1]);
    }
    if (argc >= 3) {
        maskingPercentage = std::stod(argv[2]);
    }
    if (argc >= 4) {
        probeBudgetPercent = std::stod(argv[3]);
    }
    if (argc >= 5) {
        outputFileName = argv[4];
    }

    // Generate description if not provided
    if (configDescription.empty()) {
        configDescription = "Masking: " + std::to_string((int)(maskingPercentage * 100)) + "%, " +
                          "Probe Budget: " + std::to_string((int)(probeBudgetPercent * 100)) + "%";
    }

    std::cout << "================================================================================\n";
    std::cout << "                    PUZZLE SOLVER EXPERIMENT                                    \n";
    std::cout << "================================================================================\n\n";
    std::cout << "Configuration: " << configDescription << "\n";
    std::cout << "Loading " << numPuzzles << " puzzles from " << puzzleFileName << "...\n";
    std::cout << "Masking: " << (maskingPercentage * 100) << "%, Probe Budget: " << (probeBudgetPercent * 100) << "%\n";

    // Load puzzles with specified masking percentage
    std::vector<Graph> graphs;
    PuzzleManager::loadFromFile(puzzleFileName, numPuzzles, graphs, maskingPercentage);
    std::cout << "✓ Loaded " << graphs.size() << " puzzles\n";

    // Load solutions
    auto solutionsPos = PuzzleSolver::loadSolutions(solutionsFileName);
    std::cout << "✓ Loaded solutions\n\n";

    std::cout << "Starting solver experiments...\n";
    std::cout << "--------------------------------------------------------------------------------\n";

    // Storage for all puzzle statistics
    std::vector<PuzzleStatistics> allStatistics;

    int puzzleNumber = 1;
    for (auto &g : graphs)
    {
        std::cout << "Puzzle " << std::setw(3) << puzzleNumber << "/" << numPuzzles << " ... ";
        std::cout.flush();

        PuzzleSolver solver(g);
        bool solved = solver.solvePuzzle(g.getSize(), probeBudgetPercent);

        // Collect statistics
        std::vector<std::pair<int, int>> correctPositions;
        if (solutionsPos.find(puzzleNumber) != solutionsPos.end()) {
            correctPositions = solutionsPos[puzzleNumber];
        }

        PuzzleStatistics stats = solver.collectStatistics(puzzleNumber, solved, correctPositions);
        allStatistics.push_back(stats);

        if (solved) {
            std::cout << "SOLVED";
        } else {
            std::cout << "FAILED";
        }

        std::cout << " (Q:" << stats.queensPlaced << "/" << stats.expectedQueens
                  << " P:" << stats.probesUsed << "/" << stats.probeBudget
                  << " I:" << stats.inferences
                  << " C:" << std::fixed << std::setprecision(0) << (stats.correctnessScore * 100) << "%)\n";

        puzzleNumber++;
    }

    std::cout << "--------------------------------------------------------------------------------\n";
    std::cout << "All puzzles processed. Calculating aggregate statistics...\n";

    // Calculate aggregate statistics
    AggregateStatistics aggStats = calculateAggregateStats(allStatistics);

    // Write to file (append mode with test identifiers)
    writeStatisticsToFile(outputFileName, aggStats, configDescription,
                         numPuzzles, maskingPercentage, probeBudgetPercent);

    // Also print summary to console
    std::cout << "\n================================================================================\n";
    std::cout << "                           SUMMARY RESULTS                                      \n";
    std::cout << "================================================================================\n\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Success Rate:                " << aggStats.successRate << "% ("
              << aggStats.solvedPuzzles << "/" << aggStats.totalPuzzles << ")\n";
    std::cout << "Average Correctness:         " << aggStats.avgCorrectnessAll << "%\n";
    std::cout << "Failed Puzzle Correctness:   " << aggStats.failedPuzzleCorrectness << "%\n";
    std::cout << "Probe Budget Utilization:    " << aggStats.avgProbeBudgetUtilization << "%\n";
    std::cout << "Probe-to-Inference Ratio:    1:" << aggStats.probeInferenceRatio << "\n";
    std::cout << "Cells Revealed:              " << aggStats.avgRevealPercentage << "% of masked\n\n";

    // Cleanup
    graphs.clear();

    std::cout << "Experiment complete!\n";
    std::cout << "================================================================================\n";

    return 0;
}
