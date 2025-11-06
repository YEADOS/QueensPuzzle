#include "../include/PuzzleSolver.h"
#include "../include/graph.h"
#include <set>
#include <map>
#include <algorithm>
#include <sstream>

const int PuzzleSolver::directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

PuzzleSolver::PuzzleSolver(Graph &graph) : puzzle(graph) {}

int PuzzleSolver::inferNeighbours(int row, int col)
{
    int n = puzzle.getOriginal().size();
    std::set<int> neighbours;

    for (int i = 0; i < 4; i++)
    {
        int newRow = row + directions[i][0];
        int newCol = col + directions[i][1];

        if (newRow >= 0 && newRow < n && newCol >= 0 && newCol < n)
        {
            int neighbourColour = puzzle.getMasked()[newRow][newCol];
            if (neighbourColour != -1)
            {
                neighbours.insert(neighbourColour);
            }
            else
            {
                return -1;
            }
        }
    }
    
    if (neighbours.size() == 1)
    {
        return *neighbours.begin();
    }
    return -1;
}

int PuzzleSolver::inferStrict(int row, int col)
{
    std::map<int, float> colourConfidence;

    int neighbourInfer = inferNeighbours(row, col);
    if (neighbourInfer != -1)
    {
        colourConfidence[neighbourInfer] += 3.0;
    }

    int uniformInfer = inferRowColumnUniformity(row, col);
    if (uniformInfer != -1)
    {
        colourConfidence[uniformInfer] += 2.5;
    }

    int domainInfer = inferFromDomains(row, col);
    if (domainInfer != -1)
    {
        colourConfidence[domainInfer] += 2.0;
    }

    int contigInfer = inferFromContiguity(row, col);
    if (contigInfer != -1)
    {
        colourConfidence[contigInfer] += 2.0;
    }

    int patternInfer = inferPatternCompletion(row, col);
    if (patternInfer != -1)
    {
        colourConfidence[patternInfer] += 1.5;
    }

    int bestColour = -1;
    float maxConfidence = 4.5;

    for (auto &[color, confidence] : colourConfidence)
    {
        if (confidence > maxConfidence)
        {
            maxConfidence = confidence;
            bestColour = color;
        }
    }

    return bestColour;
}

int PuzzleSolver::inferPatternCompletion(int row, int col)
{
    int n = puzzle.getOriginal().size();

    if (row > 0 && col > 0)
    {
        int above = puzzle.getMasked()[row - 1][col];
        int left = puzzle.getMasked()[row][col - 1];
        if (above != -1 && above == left)
        {
            return above;
        }
    }

    return -1;
}

int PuzzleSolver::inferRowColumnUniformity(int row, int col)
{
    int n = puzzle.getOriginal().size();

    std::map<int, int> rowColours;
    int unknownCountRow = 0;
    for (int c = 0; c < n; c++)
    {
        int colour = puzzle.getMasked()[row][c];
        if (colour == -1)
            unknownCountRow++;
        else
            rowColours[colour]++;
    }

    if (rowColours.size() == 1 && unknownCountRow > 0)
    {
        int dominantColour = rowColours.begin()->first;
        if (rowColours[dominantColour] >= n / 2)
        {
            return dominantColour;
        }
    }

    std::map<int, int> colColours;
    int unknownCountCol = 0;
    for (int r = 0; r < n; r++)
    {
        int colour = puzzle.getMasked()[r][col];
        if (colour == -1)
            unknownCountCol++;
        else
            colColours[colour]++;
    }

    if (colColours.size() == 1 && unknownCountCol > 0)
    {
        int dominantColour = colColours.begin()->first;
        if (colColours[dominantColour] >= n / 2)
        {
            return dominantColour;
        }
    }

    return -1;
}

void PuzzleSolver::computeColourDomains(std::map<int, ColourDomain> &domains)
{
    int n = puzzle.getOriginal().size();

    for (int row = 0; row < n; row++)
    {
        for (int col = 0; col < n; col++)
        {
            int colour = puzzle.getMasked()[row][col];
            if (colour != -1)
            {
                domains[colour].minRow = std::min(domains[colour].minRow, row);
                domains[colour].maxRow = std::max(domains[colour].maxRow, row);
                domains[colour].minCol = std::min(domains[colour].minCol, col);
                domains[colour].maxCol = std::max(domains[colour].maxCol, col);
            }
        }
    }
}

int PuzzleSolver::inferFromDomains(int row, int col)
{
    std::map<int, ColourDomain> domains;
    computeColourDomains(domains);

    std::vector<int> possibleColours;
    for (auto &[colour, domain] : domains)
    {
        if (domain.contains(row, col))
        {
            possibleColours.push_back(colour);
        }
    }

    if (possibleColours.size() == 1)
    {
        return possibleColours[0];
    }

    return -1;
}

int PuzzleSolver::inferFromContiguity(int row, int col)
{
    int n = puzzle.getOriginal().size();

    std::map<int, std::vector<std::pair<int, int>>> colourRegions;

    for (int i = 0; i < 4; i++)
    {
        int nr = row + directions[i][0];
        int nc = col + directions[i][1];

        if (nr >= 0 && nr < n && nc >= 0 && nc < n)
        {
            int colour = puzzle.getMasked()[nr][nc];
            if (colour != -1)
            {
                colourRegions[colour].push_back({nr, nc});
            }
        }
    }

    for (auto &[colour, cells] : colourRegions)
    {
        if (cells.size() >= 2)
        {
            bool disconnected = true;
            for (size_t i = 0; i < cells.size() - 1; i++)
            {
                for (size_t j = i + 1; j < cells.size(); j++)
                {
                    int dr = abs(cells[i].first - cells[j].first);
                    int dc = abs(cells[i].second - cells[j].second);
                    if (dr + dc == 1)
                    {
                        disconnected = false;
                        break;
                    }
                }
            }

            if (disconnected)
            {
                return colour;
            }
        }
    }

    return -1;
}

int PuzzleSolver::countUnknownNeighbours(int row, int col, int n)
{
    int unknownCount = 0;
    for (int i = 0; i < 4; i++)
    {
        int newRow = row + directions[i][0];
        int newCol = col + directions[i][1];

        if (newRow >= 0 && newRow < n && newCol >= 0 && newCol < n)
        {
            if (puzzle.getMasked()[newRow][newCol] == -1)
            {
                unknownCount++;
            }
        }
    }
    return unknownCount;
}

double PuzzleSolver::calculateProbeValue(int row, int col, int n)
{
    double value = 0.0;

    int unknownNeighbours = countUnknownNeighbours(row, col, n);
    value += unknownNeighbours * 2.0;

    if ((row == 0 || row == n-1) && (col == 0 || col == n-1))
        value += 1.5;
    else if (row == 0 || row == n-1 || col == 0 || col == n-1)
        value += 1.0;

    std::set<int> neighbourColours;
    for (int i = 0; i < 4; i++)
    {
        int newRow = row + directions[i][0];
        int newCol = col + directions[i][1];

        if (newRow >= 0 && newRow < n && newCol >= 0 && newCol < n)
        {
            int colour = puzzle.getMasked()[newRow][newCol];
            if (colour != -1)
            {
                neighbourColours.insert(colour);
            }
        }
    }
    if (neighbourColours.size() >= 2)
        value += neighbourColours.size() * 1.5;

    bool rowHasQueen = false;
    for (int c = 0; c < n; c++)
    {
        if (puzzle.getCurrentState()[row][c] == 0)
        {
            rowHasQueen = true;
            break;
        }
    }
    if (!rowHasQueen)
        value += 2.0;

    return value;
}

void PuzzleSolver::performInferenceCascade(int n)
{
    bool madeProgress = true;
    int maxPasses = 2;
    int passes = 0;

    while (madeProgress && passes < maxPasses)
    {
        madeProgress = false;
        passes++;

        for (int row = 0; row < n; row++)
        {
            for (int col = 0; col < n; col++)
            {
                if (puzzle.getMasked()[row][col] == -1)
                {
                    int inferredColour = inferStrict(row, col);
                    if (inferredColour != -1)
                    {
                        puzzle.getMasked()[row][col] = inferredColour;
                        inferredCount++;
                        madeProgress = true;
                    }
                }
            }
        }
    }
}

bool PuzzleSolver::hasQueenInColour(int color)
{
    int n = puzzle.getOriginal().size();
    for (int row = 0; row < n; row++)
    {
        for (int col = 0; col < n; col++)
        {
            if (puzzle.getCurrentState()[row][col] == 0)
            {
                int queenColour = puzzle.getMasked()[row][col];
                if (queenColour == color)
                {
                    return true;
                }
            }
        }
    }
    return false;
}

bool PuzzleSolver::validateFinalSolution(std::vector<std::pair<int, int>>& queenPositions)
{
    int n = puzzle.getOriginal().size();

    for (auto [row, col] : queenPositions) {
        if (puzzle.getMasked()[row][col] == -1) {
            probe(row, col);
        }
    }

    for (size_t i = 0; i < queenPositions.size(); i++) {
        int r1 = queenPositions[i].first;
        int c1 = queenPositions[i].second;
        int colour1 = puzzle.getMasked()[r1][c1];

        for (size_t j = i + 1; j < queenPositions.size(); j++) {
            int r2 = queenPositions[j].first;
            int c2 = queenPositions[j].second;
            int colour2 = puzzle.getMasked()[r2][c2];

            if (colour1 == colour2) {
                return false;
            }
        }
    }

    return true;
}

void PuzzleSolver::probe(int row, int col)
{
    probeCount++;
    int colour = puzzle.getOriginal()[row][col];
    puzzle.getMasked()[row][col] = colour;
}

bool PuzzleSolver::isValid(int row, int col)
{
    int n = puzzle.getOriginal().size();
    int currentColour = puzzle.getMasked()[row][col];

    if (currentColour == -1)
    {
        return false;
    }

    for (int i = 0; i < row; i++)
    {
        if (puzzle.getCurrentState()[i][col] == 0)
        {
            return false;
        }
    }

    for (int i = 0; i < row; i++) {
        for (int j = 0; j < n; j++) {
            if (puzzle.getCurrentState()[i][j] == 0) {
                if (abs(row - i) == 1 && abs(col - j) == 1) {
                    return false;
                }
            }
        }
    }

    for (int i = 0; i < row; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            if (puzzle.getCurrentState()[i][j] == 0)
            {
                int queenColour = puzzle.getMasked()[i][j];
                if (queenColour != -1 && queenColour == currentColour)
                {
                    return false;
                }
            }
        }
    }
    return true;
}

void PuzzleSolver::printStatistics()
{
    std::cout << "\n\n[ Solver Statistics ]\n";

    std::cout << "\n--- CSP Backtracking ---" << std::endl;
    std::cout << "Final queens placed: " << queensPlaced << '\n';
    std::cout << "Total Queen placement attempts: " << totalQueensPlaced << '\n';
    std::cout << "Backtracks: " << backtrackCount << '\n';

    std::cout << "\n---Robot Sensing Operations ---\n";
    std::cout << "Initial unknown cells: " << initialUnknownCells << '\n';
    std::cout << "Probe operations: " << probeCount << '\n';
    std::cout << "Inferred operations: " << inferredCount << '\n';
    
    int totalRevealed = probeCount + inferredCount;
    double revealPercentage = (double)totalRevealed / initialUnknownCells * 100.0;
    std::cout << "Total cells revealed: " << totalRevealed << " / " << initialUnknownCells << " (" << revealPercentage << "%)" << '\n';
    std::cout << "Cells still unknown: " << (initialUnknownCells - totalRevealed) << " (" << (100.0 - revealPercentage) << "%)" << '\n';

    if (probeBudget > 0) {
        std::cout << "\n--- Probe Budget ---\n";
        std::cout << "Probe budget: " << probeBudget << " ("<< (double)probeBudget/initialUnknownCells*100 << "% of unknowns)" << '\n';
        std::cout << "Probes used: " << probeCount << " / " << probeBudget;
        if (budgetExhausted) {
            std::cout << " (BUDGET FULLY USED)";
        }
        std::cout << '\n';
        std::cout << "Budget remaining: " << (probeBudget - probeCount) << '\n';


    }

    double activeSensingRatio = (double)probeCount / (inferredCount + probeCount + 1);
    double inferenceRatio = (double)inferredCount / (inferredCount + probeCount + 1);

    std::cout << "\n--- Efficiency Metrics ---\n";
    std::cout << "Active Sensing ratio: " << activeSensingRatio << " (probeCount / total sensing)\n";
    std::cout << "Inference ratio: " << inferenceRatio << " (inferred / total sensing)\n";
    std::cout << "Total operations: " << (totalQueensPlaced + backtrackCount + probeCount + inferredCount) << '\n';
}

std::vector<std::pair<int, int>> PuzzleSolver::findViableQueenPositions(int row, int n)
{
    std::vector<std::pair<int, int>> viablePositions;

    for (int col = 0; col < n; col++) {
        bool basicConstraintsOK = true;

        // Check if any queen already exists in this column
        for (int r = 0; r < row; r++) {
            if (puzzle.getCurrentState()[r][col] == 0) {
                basicConstraintsOK = false;
                break;
            }
        }

        // Check for queens on diagonals
        if (basicConstraintsOK) {
            for (int r = 0; r < row; r++) {
                for (int c = 0; c < n; c++) {
                    if (puzzle.getCurrentState()[r][c] == 0) {
                        if (abs(row - r) == 1 && abs(col - c) == 1) {
                            basicConstraintsOK = false;
                            break;
                        }
                    }
                }
                if (!basicConstraintsOK) break;
            }
        }

        // Infer or verify cell color
        if (basicConstraintsOK) {
            int cellColour = puzzle.getMasked()[row][col];

            if (cellColour == -1) {
                int inferredColour = inferStrict(row, col);
                if (inferredColour != -1) {
                    puzzle.getMasked()[row][col] = inferredColour;
                    inferredCount++;
                    cellColour = inferredColour;
                }
            }

            // Check if this color is already used by another queen
            if (cellColour != -1) {
                if (hasQueenInColour(cellColour)) {
                    basicConstraintsOK = false;
                }
            }
        }

        // Add position if all constraints pass
        if (basicConstraintsOK) {
            viablePositions.push_back({row, col});
        }
    }

    return viablePositions;
}

void PuzzleSolver::undoQueenPlacement(int row, int col)
{
    puzzle.getCurrentState()[row][col] = puzzle.getMasked()[row][col];
    queensPlaced--;
    backtrackCount++;
}

void PuzzleSolver::restoreBestPartialSolution()
{
    int n = puzzle.getOriginal().size();

    for (int row = 0; row < n; row++) {
        for (int col = 0; col < n; col++) {
            puzzle.getCurrentState()[row][col] = puzzle.getMasked()[row][col];
        }
    }

    for (auto [row, col] : bestPartialSolution) {
        puzzle.getCurrentState()[row][col] = 0;
    }
}

double PuzzleSolver::calculateExpectedInformationGain(int row, int col, int n)
{
    double gain = 0.0;

    gain += 1.0;

    int unknownNeighbours = countUnknownNeighbours(row, col, n);
    gain += unknownNeighbours * 0.5;

    gain += calculateProbeValue(row, col, n) * 0.1;

    std::set<int> neighbourColours;
    for (int i = 0; i < 4; i++) {
        int nr = row + directions[i][0];
        int nc = col + directions[i][1];
        if (nr >= 0 && nr < n && nc >= 0 && nc < n) {
            int colour = puzzle.getMasked()[nr][nc];
            if (colour != -1) {
                neighbourColours.insert(colour);
            }
        }
    }
    gain += neighbourColours.size() * 0.3;

    return gain;
}

std::vector<std::pair<int, int>> PuzzleSolver::findBestProbeSpots(
    int k, std::vector<std::pair<int, int>>& viablePositions)
{
    int n = puzzle.getOriginal().size();
    std::vector<std::pair<double, std::pair<int, int>>> scoredProbes;

    std::set<std::pair<int, int>> candidates;

    for (auto [row, col] : viablePositions) {
        if (puzzle.getMasked()[row][col] == -1) {
            candidates.insert({row, col});
        }

        for (int i = 0; i < 4; i++) {
            int nr = row + directions[i][0];
            int nc = col + directions[i][1];
            if (nr >= 0 && nr < n && nc >= 0 && nc < n) {
                if (puzzle.getMasked()[nr][nc] == -1) {
                    candidates.insert({nr, nc});
                }
            }
        }
    }

    for (auto [row, col] : candidates) {
        double gain = calculateExpectedInformationGain(row, col, n);
        scoredProbes.push_back({gain, {row, col}});
    }

    std::sort(scoredProbes.begin(), scoredProbes.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });

    std::vector<std::pair<int, int>> result;
    for (int i = 0; i < std::min(k, (int)scoredProbes.size()); i++) {
        result.push_back(scoredProbes[i].second);
    }

    return result;
}

void PuzzleSolver::propagateConstraints(int n)
{
    performInferenceCascade(n);
}

bool PuzzleSolver::solvePuzzle(int n)
{
    setProbeBudget(n, 0.5);

    bestPartialSolution.clear();
    maxQueensPlaced = 0;

    std::vector<std::pair<int, int>> queenPositions;
    bool solved = mainSolver(0, n, queenPositions);

    if (!solved && !bestPartialSolution.empty()) {
        restoreBestPartialSolution();
    }

    return solved;
}

bool PuzzleSolver::solvePuzzle(int n, double probeBudgetPercent)
{
    setProbeBudget(n, probeBudgetPercent);

    bestPartialSolution.clear();
    maxQueensPlaced = 0;

    std::vector<std::pair<int, int>> queenPositions;
    bool solved = mainSolver(0, n, queenPositions);

    if (!solved && !bestPartialSolution.empty()) {
        restoreBestPartialSolution();
    }

    return solved;
}

bool PuzzleSolver::mainSolver(int row, int n, std::vector<std::pair<int, int>>& queenPositions)
{
    if (row == n) {
        return validateFinalSolution(queenPositions);
    }

    if (queenPositions.size() > maxQueensPlaced) {
        maxQueensPlaced = queenPositions.size();
        bestPartialSolution = queenPositions;
    }

    auto viablePositions = findViableQueenPositions(row, n);

    if (viablePositions.empty()) {
        return false;
    }

    if (canProbe()) {
        int maxProbesThisRound = std::min(2, (int)viablePositions.size());
        auto informativeProbes = findBestProbeSpots(maxProbesThisRound, viablePositions);

        for (auto [pr, pc] : informativeProbes) {
            if (!canProbe()) break;

            if (puzzle.getMasked()[pr][pc] == -1) {
                int inferredColour = inferStrict(pr, pc);
                if (inferredColour != -1) {
                    puzzle.getMasked()[pr][pc] = inferredColour;
                    inferredCount++;
                } else {
                    probe(pr, pc);
                }
            }
        }
    }

    propagateConstraints(n);

    viablePositions = findViableQueenPositions(row, n);

    if (viablePositions.empty()) {
        return false;
    }

    std::vector<std::pair<double, std::pair<int, int>>> scoredPositions;

    for (auto [row, col] : viablePositions) {
        double score = 0.0;

        if (puzzle.getMasked()[row][col] != -1) {
            score = 1000.0;
        } else {
            int inferredColour = inferStrict(row, col);
            if (inferredColour != -1) {
                score = 500.0;
            } else {
                score = calculateProbeValue(row, col, n);
            }
        }

        scoredPositions.push_back({score, {row, col}});
    }

    std::sort(scoredPositions.begin(), scoredPositions.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });

    for (auto& [score, pos] : scoredPositions) {
        int row = pos.first;
        int col = pos.second;

        int cellColour = puzzle.getMasked()[row][col];

        if (cellColour == -1) {
            int inferredColour = inferStrict(row, col);
            if (inferredColour != -1) {
                puzzle.getMasked()[row][col] = inferredColour;
                inferredCount++;
                cellColour = inferredColour;
            } else if (canProbe()) {
                probe(row, col);
                propagateConstraints(n);
                cellColour = puzzle.getMasked()[row][col];
            } else {
                double confidence = 0.0;
                int predictedColour = inferWeak(row, col, confidence);

                if (confidence >= 2.0 && predictedColour != -1) {
                    cellColour = predictedColour;
                } else {
                    continue;
                }
            }
        }

        if (cellColour == -1) continue;

        if (hasQueenInColour(cellColour)) continue;

        if (isValid(row, col)) {
            puzzle.getCurrentState()[row][col] = 0;
            queensPlaced++;
            totalQueensPlaced++;
            queenPositions.push_back({row, col});

            if (mainSolver(row + 1, n, queenPositions)) {
                return true;
            }

            undoQueenPlacement(row, col);
            queenPositions.pop_back();
        }
    }

    return false;
}

void PuzzleSolver::setProbeBudget(int n, double budgetPercent)
{
    initialUnknownCells = 0;
    // Loop through each square to count how many are unknown (masked) 
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (puzzle.getMasked()[i][j] == -1) {
                initialUnknownCells++;
            }
        }
    }
    probeBudget = static_cast<int>(initialUnknownCells * budgetPercent);
    budgetExhausted = false;
}

bool PuzzleSolver::canProbe()
{
    if (probeCount >= probeBudget) {
        budgetExhausted = true;
        return false;
    }
    return true;
}

int PuzzleSolver::inferWeak(int row, int col, double& confidence)
{
    std::map<int, float> colourConfidence;
    int n = puzzle.getSize();

    int neighbourInfer = inferNeighbours(row, col);
    if (neighbourInfer != -1) {
        colourConfidence[neighbourInfer] += 2.0;
    }

    int rowColInfer = inferRowColumnUniformity(row, col);
    if (rowColInfer != -1) {
        colourConfidence[rowColInfer] += 1.5;
    }

    int domainInfer = inferFromDomains(row, col);
    if (domainInfer != -1) {
        colourConfidence[domainInfer] += 1.0;
    }

    int contiguityInfer = inferFromContiguity(row, col);
    if (contiguityInfer != -1) {
        colourConfidence[contiguityInfer] += 1.5;
    }

    int patternInfer = inferPatternCompletion(row, col);
    if (patternInfer != -1) {
        colourConfidence[patternInfer] += 1.5;
    }

    int bestColour = -1;
    float maxConfidence = 0.0;

    for (auto &[color, conf] : colourConfidence) {
        if (conf > maxConfidence) {
            maxConfidence = conf;
            bestColour = color;
        }
    }

    confidence = maxConfidence;
    return bestColour;
}

std::map<int, std::vector<std::pair<int, int>>> PuzzleSolver::loadSolutions(const std::string& filename)
{
    std::map<int, std::vector<std::pair<int, int>>> solutions;
    std::ifstream inFile(filename);

    if (!inFile.is_open()) {
        std::cerr << "Warning: Could not open " << filename << " for ground truth verification" << std::endl;
        return solutions;
    }

    std::string line;
    int currentPuzzle = -1;
    std::vector<std::pair<int, int>> currentSolution;

    while (std::getline(inFile, line)) {
        if (line.find("PUZZLE") == 0) {
            size_t pos = line.find(' ');
            if (pos != std::string::npos) {
                std::string numStr = line.substr(pos + 1);
                size_t spacePos = numStr.find(' ');
                if (spacePos != std::string::npos) {
                    numStr = numStr.substr(0, spacePos);
                }
                currentPuzzle = std::stoi(numStr);
                currentSolution.clear();
            }
        } else if (line.find("SIZE") == 0) {
            continue;
        } else if (line == "END") {
            if (currentPuzzle != -1) {
                solutions[currentPuzzle] = currentSolution;
            }
            currentPuzzle = -1;
        } else if (currentPuzzle != -1 && !line.empty()) {
            std::istringstream iss(line);
            int row, col;
            if (iss >> row >> col) {
                currentSolution.push_back({row, col});
            }
        }
    }

    inFile.close();
    return solutions;
}

std::map<int, std::vector<std::string>> PuzzleSolver::loadVisualSolutions(const std::string& filename)
{
    std::map<int, std::vector<std::string>> visualSolutions;
    std::ifstream inFile(filename);

    if (!inFile.is_open()) {
        return visualSolutions;
    }

    std::string line;
    int currentPuzzle = -1;
    std::vector<std::string> currentBoard;

    while (std::getline(inFile, line)) {
        if (line.find("PUZZLE") == 0) {
            if (currentPuzzle != -1 && !currentBoard.empty()) {
                visualSolutions[currentPuzzle] = currentBoard;
            }
            currentBoard.clear();
            
            size_t spacePos = line.find(' ');
            size_t endPos = line.find(' ', spacePos + 1);
            currentPuzzle = std::stoi(line.substr(spacePos + 1, endPos - spacePos - 1));
        } else if (line.find("SIZE") == 0) {
            continue;
        } else if (line == "-----") {
            if (currentPuzzle != -1) {
                visualSolutions[currentPuzzle] = currentBoard;
            }
            currentPuzzle = -1;
        } else if (currentPuzzle != -1 && !line.empty() && line != "-----") {
            currentBoard.push_back(line);
        }
    }

    inFile.close();
    return visualSolutions;
}

double PuzzleSolver::compareToCorrectPositions(int puzzleNumber, const std::vector<std::pair<int, int>>& correctPositions)
{
    if (correctPositions.empty()) {
        return 0.0;
    }

    int n = puzzle.getSize();
    std::set<std::pair<int, int>> correctPositionsSet(correctPositions.begin(), correctPositions.end());
    std::set<std::pair<int, int>> currentQueens;

    for (int row = 0; row < n; row++) {
        for (int col = 0; col < n; col++) {
            if (puzzle.getCurrentState()[row][col] == 0) {
                currentQueens.insert({row, col});
            }
        }
    }

    int totalCorrect = 0;
    for (const auto& pos : currentQueens) {
        if (correctPositionsSet.find(pos) != correctPositionsSet.end()) {
            totalCorrect++;
        }
    }

    if (currentQueens.size() == correctPositionsSet.size() &&
        std::all_of(currentQueens.begin(), currentQueens.end(), [&](const std::pair<int,int>& p){
            return correctPositionsSet.find(p) != correctPositionsSet.end();
        })) {
        return 1.0;
    }

    return (double)totalCorrect / correctPositions.size();
}

void PuzzleSolver::printCorrectnessReport(int puzzleNumber, const std::vector<std::pair<int, int>>& correctPositions)
{
    if (correctPositions.empty()) {
        std::cout << "\nNo correct solutions available for comparison\n";
        return;
    }

    int n = puzzle.getSize();
    std::set<std::pair<int, int>> correctPositionsSet(correctPositions.begin(), correctPositions.end());
    std::vector<std::pair<int, int>> currentQueenPositions;

    for (int row = 0; row < n; row++) {
        for (int col = 0; col < n; col++) {
            if (puzzle.getCurrentState()[row][col] == 0) {
                currentQueenPositions.push_back({row, col});
            }
        }
    }

    std::vector<std::pair<int, int>> correct;
    std::vector<std::pair<int, int>> incorrect;
    std::vector<std::pair<int, int>> missing;

    for (const auto& pos : currentQueenPositions) {
        if (correctPositionsSet.find(pos) != correctPositionsSet.end()) {
            correct.push_back(pos);
        } else {
            incorrect.push_back(pos);
        }
    }

    for (const auto& pos : correctPositions) {
        if (std::find(currentQueenPositions.begin(), currentQueenPositions.end(), pos) == currentQueenPositions.end()) {
            missing.push_back(pos);
        }
    }

    double correctPercent = (double)correct.size() / correctPositions.size() * 100.0;

    std::cout << "\n\n[Solver vs Solution]\n";

    std::cout << "--- Comparison ---\n";
    std::cout << "Expected queens: " << correctPositions.size() << std::endl;
    std::cout << "Placed queens: " << currentQueenPositions.size() << std::endl;
    std::cout << "Correct positions: " << correct.size() << " / " << correctPositions.size() << " (" << correctPercent << "%)\n";

    bool exactMatch = (currentQueenPositions.size() == correctPositionsSet.size());
    if (exactMatch) {
        for (const auto &p : currentQueenPositions) {
            if (correctPositionsSet.find(p) == correctPositionsSet.end()) {
                exactMatch = false;
                break;
            }
        }
    }

    std::cout << "Exact positional match: " << (exactMatch ? "YES" : "NO") << '\n';

    if (!correct.empty()) {
        std::cout << "\nCorrect: ";
        for (const auto& [row, col] : correct) {
            std::cout << "(" << row << "," << col << ") ";
        }
        std::cout << '\n';
    }

    if (!incorrect.empty()) {
        std::cout << "Incorrect: ";
        for (const auto& [row, col] : incorrect) {
            std::cout << "(" << row << "," << col << ") ";
        }
        std::cout << '\n';
    }

    if (!missing.empty()) {
        std::cout << "Missing: ";
        for (const auto& [row, col] : missing) {
            std::cout << "(" << row << "," << col << ") ";
        }
        std::cout << '\n';
    }

    if (correctPercent == 100.0) {
        std::cout << "\nSolver solution is correct\n";
    }
}

PuzzleStatistics PuzzleSolver::collectStatistics(int puzzleNumber, bool solved,
                                                  const std::vector<std::pair<int, int>>& correctPositions)
{
    PuzzleStatistics stats;
    stats.puzzleNumber = puzzleNumber;
    stats.solved = solved;
    stats.gridSize = puzzle.getSize();
    stats.queensPlaced = queensPlaced;
    stats.expectedQueens = puzzle.getSize();
    stats.probesUsed = probeCount;
    stats.probeBudget = probeBudget;
    stats.inferences = inferredCount;
    stats.backtracks = backtrackCount;
    stats.initialMaskedCells = initialUnknownCells;
    stats.cellsRevealed = probeCount + inferredCount;

    // Calculate correctness score
    if (!correctPositions.empty()) {
        stats.correctnessScore = compareToCorrectPositions(puzzleNumber, correctPositions);

        // For failed puzzles, count correct queens placed
        if (!solved) {
            int n = puzzle.getSize();
            std::set<std::pair<int, int>> correctPositionsSet(correctPositions.begin(), correctPositions.end());

            stats.correctQueens = 0;
            for (int row = 0; row < n; row++) {
                for (int col = 0; col < n; col++) {
                    if (puzzle.getCurrentState()[row][col] == 0) {
                        if (correctPositionsSet.find({row, col}) != correctPositionsSet.end()) {
                            stats.correctQueens++;
                        }
                    }
                }
            }
        }
    }

    return stats;
}
