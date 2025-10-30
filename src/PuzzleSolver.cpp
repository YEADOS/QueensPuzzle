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

int PuzzleSolver::inferWithConfidence(int row, int col)
{
    std::map<int, float> colorConfidence;

    int neighborInfer = inferNeighbours(row, col);
    if (neighborInfer != -1)
    {
        colorConfidence[neighborInfer] += 3.0;
    }

    int uniformInfer = inferRowColumnUniformity(row, col);
    if (uniformInfer != -1)
    {
        colorConfidence[uniformInfer] += 2.5;
    }

    int domainInfer = inferFromDomains(row, col);
    if (domainInfer != -1)
    {
        colorConfidence[domainInfer] += 2.0;
    }

    int contigInfer = inferFromContiguity(row, col);
    if (contigInfer != -1)
    {
        colorConfidence[contigInfer] += 2.0;
    }

    int patternInfer = inferPatternCompletion(row, col);
    if (patternInfer != -1)
    {
        colorConfidence[patternInfer] += 1.5;
    }

    int bestColor = -1;
    float maxConfidence = 4.5;

    for (auto &[color, confidence] : colorConfidence)
    {
        if (confidence > maxConfidence)
        {
            maxConfidence = confidence;
            bestColor = color;
        }
    }

    return bestColor;
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

    std::map<int, int> rowColors;
    int unknownCountRow = 0;
    for (int c = 0; c < n; c++)
    {
        int color = puzzle.getMasked()[row][c];
        if (color == -1)
            unknownCountRow++;
        else
            rowColors[color]++;
    }

    if (rowColors.size() == 1 && unknownCountRow > 0)
    {
        int dominantColor = rowColors.begin()->first;
        if (rowColors[dominantColor] >= n / 2)
        {
            return dominantColor;
        }
    }

    std::map<int, int> colColors;
    int unknownCountCol = 0;
    for (int r = 0; r < n; r++)
    {
        int color = puzzle.getMasked()[r][col];
        if (color == -1)
            unknownCountCol++;
        else
            colColors[color]++;
    }

    if (colColors.size() == 1 && unknownCountCol > 0)
    {
        int dominantColor = colColors.begin()->first;
        if (colColors[dominantColor] >= n / 2)
        {
            return dominantColor;
        }
    }

    return -1;
}

void PuzzleSolver::computeColorDomains(std::map<int, ColorDomain> &domains)
{
    int n = puzzle.getOriginal().size();

    for (int r = 0; r < n; r++)
    {
        for (int c = 0; c < n; c++)
        {
            int color = puzzle.getMasked()[r][c];
            if (color != -1)
            {
                domains[color].minRow = std::min(domains[color].minRow, r);
                domains[color].maxRow = std::max(domains[color].maxRow, r);
                domains[color].minCol = std::min(domains[color].minCol, c);
                domains[color].maxCol = std::max(domains[color].maxCol, c);
            }
        }
    }
}

int PuzzleSolver::inferFromDomains(int row, int col)
{
    std::map<int, ColorDomain> domains;
    computeColorDomains(domains);

    std::vector<int> possibleColors;
    for (auto &[color, domain] : domains)
    {
        if (domain.contains(row, col))
        {
            possibleColors.push_back(color);
        }
    }

    if (possibleColors.size() == 1)
    {
        return possibleColors[0];
    }

    return -1;
}

int PuzzleSolver::inferFromContiguity(int row, int col)
{
    int n = puzzle.getOriginal().size();

    std::map<int, std::vector<std::pair<int, int>>> colorRegions;

    for (int i = 0; i < 4; i++)
    {
        int nr = row + directions[i][0];
        int nc = col + directions[i][1];

        if (nr >= 0 && nr < n && nc >= 0 && nc < n)
        {
            int color = puzzle.getMasked()[nr][nc];
            if (color != -1)
            {
                colorRegions[color].push_back({nr, nc});
            }
        }
    }

    for (auto &[color, cells] : colorRegions)
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
                return color;
            }
        }
    }

    return -1;
}

int PuzzleSolver::countUnknownNeighbors(int row, int col, int n)
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

    int unknownNeighbors = countUnknownNeighbors(row, col, n);
    value += unknownNeighbors * 2.0;

    if ((row == 0 || row == n-1) && (col == 0 || col == n-1))
        value += 1.5;
    else if (row == 0 || row == n-1 || col == 0 || col == n-1)
        value += 1.0;

    std::set<int> neighborColors;
    for (int i = 0; i < 4; i++)
    {
        int newRow = row + directions[i][0];
        int newCol = col + directions[i][1];

        if (newRow >= 0 && newRow < n && newCol >= 0 && newCol < n)
        {
            int color = puzzle.getMasked()[newRow][newCol];
            if (color != -1)
            {
                neighborColors.insert(color);
            }
        }
    }
    if (neighborColors.size() >= 2)
        value += neighborColors.size() * 1.5;

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

        for (int r = 0; r < n; r++)
        {
            for (int c = 0; c < n; c++)
            {
                if (puzzle.getMasked()[r][c] == -1)
                {
                    int inferredColor = inferWithConfidence(r, c);
                    if (inferredColor != -1)
                    {
                        puzzle.getMasked()[r][c] = inferredColor;
                        inferredCount++;
                        madeProgress = true;
                    }
                }
            }
        }
    }
}

bool PuzzleSolver::hasQueenInColor(int color)
{
    int n = puzzle.getOriginal().size();
    for (int r = 0; r < n; r++)
    {
        for (int c = 0; c < n; c++)
        {
            if (puzzle.getCurrentState()[r][c] == 0)
            {
                int queenColor = puzzle.getMasked()[r][c];
                if (queenColor == color)
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

    for (auto [r, c] : queenPositions) {
        if (puzzle.getMasked()[r][c] == -1) {
            probe(r, c);
        }
    }

    for (size_t i = 0; i < queenPositions.size(); i++) {
        int r1 = queenPositions[i].first;
        int c1 = queenPositions[i].second;
        int color1 = puzzle.getMasked()[r1][c1];

        for (size_t j = i + 1; j < queenPositions.size(); j++) {
            int r2 = queenPositions[j].first;
            int c2 = queenPositions[j].second;
            int color2 = puzzle.getMasked()[r2][c2];

            if (color1 == color2) {
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
                int queenColor = puzzle.getMasked()[i][j];
                if (queenColor != -1 && queenColor == currentColour)
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
    std::cout << "\n=== MINIMAL SENSING Statistics ===" << std::endl;
    std::cout << "Final queens placed: " << queensPlaced << std::endl;
    std::cout << "Total placement attempts: " << totalQueensPlaced << std::endl;
    std::cout << "Backtracks: " << backtrackCount << std::endl;

    if (probeBudget > 0) {
        std::cout << "\n--- Probe Budget ---" << std::endl;
        std::cout << "Initially unknown cells: " << initialUnknownCells << std::endl;
        std::cout << "Probe budget: " << probeBudget << " ("
                  << (double)probeBudget/initialUnknownCells*100 << "% of unknowns)" << std::endl;
        std::cout << "Probes used: " << probeCount << " / " << probeBudget;
        if (budgetExhausted) {
            std::cout << " (BUDGET EXHAUSTED)";
        }
        std::cout << std::endl;
        std::cout << "Budget remaining: " << (probeBudget - probeCount) << std::endl;

        int totalRevealed = probeCount + inferredCount;
        double revelationPercent = (double)totalRevealed / initialUnknownCells * 100.0;
        std::cout << "Total cells revealed: " << totalRevealed << " / " << initialUnknownCells
                  << " (" << revelationPercent << "%)" << std::endl;
        std::cout << "Cells still unknown: " << (initialUnknownCells - totalRevealed)
                  << " (" << (100.0 - revelationPercent) << "%)" << std::endl;
    }

    std::cout << "\n--- Sensing Operations ---" << std::endl;
    std::cout << "Probe operations: " << probeCount << std::endl;
    std::cout << "Inferred operations: " << inferredCount << std::endl;

    double efficiency = (double)queensPlaced / (totalQueensPlaced + 1);
    double inferenceRatio = (double)inferredCount / (inferredCount + probeCount + 1);

    std::cout << "\n--- Efficiency Metrics ---" << std::endl;
    std::cout << "Search efficiency: " << efficiency << " (final queens / total attempts)" << std::endl;
    std::cout << "Inference ratio: " << inferenceRatio << " (inferred / total sensing)" << std::endl;
    std::cout << "Total operations: " << (totalQueensPlaced + backtrackCount + probeCount + inferredCount) << std::endl;
}

void PuzzleSolver::verifyQueenPlacement()
{
    int n = puzzle.getOriginal().size();
    std::vector<std::pair<int, int>> queensPositions;

    for (int r = 0; r < n; r++) {
        for (int c = 0; c < n; c++) {
            if (puzzle.getCurrentState()[r][c] == 0) {
                queensPositions.push_back({r, c});
            }
        }
    }

    std::cout << "\n=== QUEEN PLACEMENT VERIFICATION ===" << std::endl;
    std::cout << "Found " << queensPositions.size() << " queens (should be " << n << ")" << std::endl;

    if (queensPositions.size() != n) {
        std::cout << "WRONG NUMBER OF QUEENS!" << std::endl;
        return;
    }

    bool hasViolations = false;
    for (auto [r, c] : queensPositions) {
        int queenColor = puzzle.getMasked()[r][c];
        std::cout << "Queen at (" << r << "," << c << ") in color " << queenColor << std::endl;

        for (auto [r2, c2] : queensPositions) {
            if (r2 != r || c2 != c) {
                if (puzzle.getMasked()[r2][c2] == queenColor) {
                    std::cout << "VIOLATION: Multiple queens in same color " << queenColor
                             << " at (" << r << "," << c << ") and (" << r2 << "," << c2 << ")" << std::endl;
                    hasViolations = true;
                }

                if (c2 == c) {
                    std::cout << "VIOLATION: Multiple queens in same column " << c
                             << " at (" << r << "," << c << ") and (" << r2 << "," << c2 << ")" << std::endl;
                    hasViolations = true;
                }

                if (abs(r2 - r) == 1 && abs(c2 - c) == 1) {
                    std::cout << "VIOLATION: Queens diagonally adjacent (" << r << "," << c
                             << ") and (" << r2 << "," << c2 << ")" << std::endl;
                    hasViolations = true;
                }
            }
        }
    }

    if (hasViolations) {
        std::cout << "SOLUTION HAS CONSTRAINT VIOLATIONS!" << std::endl;
    } else {
        std::cout << "All constraints satisfied" << std::endl;
    }

    std::cout << "Verification complete" << std::endl;
}

std::vector<std::pair<int, int>> PuzzleSolver::findViableQueenPositions(int row, int n)
{
    std::vector<std::pair<int, int>> viablePositions;

    for (int col = 0; col < n; col++) {
        bool basicConstraintsOK = true;

        for (int r = 0; r < row; r++) {
            if (puzzle.getCurrentState()[r][col] == 0) {
                basicConstraintsOK = false;
                break;
            }
        }

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

        if (basicConstraintsOK) {
            int cellColor = puzzle.getMasked()[row][col];

            if (cellColor == -1) {
                int inferredColor = inferWithConfidence(row, col);
                if (inferredColor != -1) {
                    puzzle.getMasked()[row][col] = inferredColor;
                    inferredCount++;
                    cellColor = inferredColor;
                }
            }

            if (cellColor != -1) {
                if (hasQueenInColor(cellColor)) {
                    basicConstraintsOK = false;
                }
            }
        }

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

    for (int r = 0; r < n; r++) {
        for (int c = 0; c < n; c++) {
            puzzle.getCurrentState()[r][c] = puzzle.getMasked()[r][c];
        }
    }

    for (auto [r, c] : bestPartialSolution) {
        puzzle.getCurrentState()[r][c] = 0;
    }
}

double PuzzleSolver::calculateExpectedInformationGain(int row, int col, int n)
{
    double gain = 0.0;

    gain += 1.0;

    int unknownNeighbors = countUnknownNeighbors(row, col, n);
    gain += unknownNeighbors * 0.5;

    gain += calculateProbeValue(row, col, n) * 0.1;

    std::set<int> neighborColors;
    for (int i = 0; i < 4; i++) {
        int nr = row + directions[i][0];
        int nc = col + directions[i][1];
        if (nr >= 0 && nr < n && nc >= 0 && nc < n) {
            int color = puzzle.getMasked()[nr][nc];
            if (color != -1) {
                neighborColors.insert(color);
            }
        }
    }
    gain += neighborColors.size() * 0.3;

    return gain;
}

std::vector<std::pair<int, int>> PuzzleSolver::getMostInformativeProbes(
    int k, std::vector<std::pair<int, int>>& viablePositions)
{
    int n = puzzle.getOriginal().size();
    std::vector<std::pair<double, std::pair<int, int>>> scoredProbes;

    std::set<std::pair<int, int>> candidates;

    for (auto [r, c] : viablePositions) {
        if (puzzle.getMasked()[r][c] == -1) {
            candidates.insert({r, c});
        }

        for (int i = 0; i < 4; i++) {
            int nr = r + directions[i][0];
            int nc = c + directions[i][1];
            if (nr >= 0 && nr < n && nc >= 0 && nc < n) {
                if (puzzle.getMasked()[nr][nc] == -1) {
                    candidates.insert({nr, nc});
                }
            }
        }
    }

    for (auto [r, c] : candidates) {
        double gain = calculateExpectedInformationGain(r, c, n);
        scoredProbes.push_back({gain, {r, c}});
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
    initializeProbeBudget(n, 1);

    bestPartialSolution.clear();
    maxQueensPlaced = 0;

    std::vector<std::pair<int, int>> queenPositions;
    bool solved = solveActiveCSPBacktrack(0, n, queenPositions);

    if (!solved && !bestPartialSolution.empty()) {
        restoreBestPartialSolution();
    }

    return solved;
}

bool PuzzleSolver::solveActiveCSPBacktrack(int row, int n, std::vector<std::pair<int, int>>& queenPositions)
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
        auto informativeProbes = getMostInformativeProbes(maxProbesThisRound, viablePositions);

        for (auto [pr, pc] : informativeProbes) {
            if (!canProbe()) break;

            if (puzzle.getMasked()[pr][pc] == -1) {
                int inferredColor = inferWithConfidence(pr, pc);
                if (inferredColor != -1) {
                    puzzle.getMasked()[pr][pc] = inferredColor;
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

    for (auto [r, c] : viablePositions) {
        double score = 0.0;

        if (puzzle.getMasked()[r][c] != -1) {
            score = 1000.0;
        } else {
            int inferredColor = inferWithConfidence(r, c);
            if (inferredColor != -1) {
                score = 500.0;
            } else {
                score = calculateProbeValue(r, c, n);
            }
        }

        scoredPositions.push_back({score, {r, c}});
    }

    std::sort(scoredPositions.begin(), scoredPositions.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });

    for (auto& [score, pos] : scoredPositions) {
        int r = pos.first;
        int c = pos.second;

        int cellColor = puzzle.getMasked()[r][c];

        if (cellColor == -1) {
            int inferredColor = inferWithConfidence(r, c);
            if (inferredColor != -1) {
                puzzle.getMasked()[r][c] = inferredColor;
                inferredCount++;
                cellColor = inferredColor;
            } else if (canProbe()) {
                probe(r, c);
                propagateConstraints(n);
                cellColor = puzzle.getMasked()[r][c];
            } else {
                double confidence = 0.0;
                int predictedColor = getMostLikelyColor(r, c, confidence);

                if (confidence >= 2.0 && predictedColor != -1) {
                    cellColor = predictedColor;
                } else {
                    continue;
                }
            }
        }

        if (cellColor == -1) continue;

        if (hasQueenInColor(cellColor)) continue;

        if (isValid(r, c)) {
            puzzle.getCurrentState()[r][c] = 0;
            queensPlaced++;
            totalQueensPlaced++;
            queenPositions.push_back({r, c});

            if (solveActiveCSPBacktrack(row + 1, n, queenPositions)) {
                return true;
            }

            undoQueenPlacement(r, c);
            queenPositions.pop_back();
        }
    }

    return false;
}

void PuzzleSolver::initializeProbeBudget(int n, double budgetPercent)
{
    initialUnknownCells = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (puzzle.getMasked()[i][j] == -1) {
                initialUnknownCells++;
            }
        }
    }

    probeBudget = static_cast<int>(initialUnknownCells * budgetPercent);
    budgetExhausted = false;

    std::cout << "[MINIMAL SENSING] Probe budget initialized:\n";
    std::cout << "  Unknown cells: " << initialUnknownCells << "\n";
    std::cout << "  Budget: " << probeBudget << " probes ("
              << (budgetPercent * 100) << "% of unknowns)\n";
}

bool PuzzleSolver::canProbe()
{
    if (probeCount >= probeBudget) {
        budgetExhausted = true;
        return false;
    }
    return true;
}

int PuzzleSolver::getMostLikelyColor(int row, int col, double& confidence)
{
    std::map<int, float> colorConfidence;
    int n = puzzle.getSize();

    int neighborInfer = inferNeighbours(row, col);
    if (neighborInfer != -1) {
        colorConfidence[neighborInfer] += 2.0;
    }

    int rowColInfer = inferRowColumnUniformity(row, col);
    if (rowColInfer != -1) {
        colorConfidence[rowColInfer] += 1.5;
    }

    int domainInfer = inferFromDomains(row, col);
    if (domainInfer != -1) {
        colorConfidence[domainInfer] += 1.0;
    }

    int contiguityInfer = inferFromContiguity(row, col);
    if (contiguityInfer != -1) {
        colorConfidence[contiguityInfer] += 1.5;
    }

    int patternInfer = inferPatternCompletion(row, col);
    if (patternInfer != -1) {
        colorConfidence[patternInfer] += 1.5;
    }

    int bestColor = -1;
    float maxConfidence = 0.0;

    for (auto &[color, conf] : colorConfidence) {
        if (conf > maxConfidence) {
            maxConfidence = conf;
            bestColor = color;
        }
    }

    confidence = maxConfidence;
    return bestColor;
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

double PuzzleSolver::compareWithGroundTruth(int puzzleNumber, const std::vector<std::pair<int, int>>& groundTruth)
{
    if (groundTruth.empty()) {
        return 0.0;
    }

    int n = puzzle.getSize();
    std::set<std::pair<int, int>> groundTruthSet(groundTruth.begin(), groundTruth.end());
    std::set<std::pair<int, int>> currentQueens;

    for (int r = 0; r < n; r++) {
        for (int c = 0; c < n; c++) {
            if (puzzle.getCurrentState()[r][c] == 0) {
                currentQueens.insert({r, c});
            }
        }
    }

    int correctPositions = 0;
    for (const auto& pos : currentQueens) {
        if (groundTruthSet.find(pos) != groundTruthSet.end()) {
            correctPositions++;
        }
    }

    if (currentQueens.size() == groundTruthSet.size() &&
        std::all_of(currentQueens.begin(), currentQueens.end(), [&](const std::pair<int,int>& p){
            return groundTruthSet.find(p) != groundTruthSet.end();
        })) {
        return 1.0;
    }

    return (double)correctPositions / groundTruth.size();
}

void PuzzleSolver::printCorrectnessReport(int puzzleNumber, const std::vector<std::pair<int, int>>& groundTruth)
{
    if (groundTruth.empty()) {
        std::cout << "\nNo ground truth solution available for comparison" << std::endl;
        return;
    }

    int n = puzzle.getSize();
    std::set<std::pair<int, int>> groundTruthSet(groundTruth.begin(), groundTruth.end());
    std::vector<std::pair<int, int>> currentQueens;

    for (int r = 0; r < n; r++) {
        for (int c = 0; c < n; c++) {
            if (puzzle.getCurrentState()[r][c] == 0) {
                currentQueens.push_back({r, c});
            }
        }
    }

    std::vector<std::pair<int, int>> correct;
    std::vector<std::pair<int, int>> incorrect;
    std::vector<std::pair<int, int>> missing;

    for (const auto& pos : currentQueens) {
        if (groundTruthSet.find(pos) != groundTruthSet.end()) {
            correct.push_back(pos);
        } else {
            incorrect.push_back(pos);
        }
    }

    for (const auto& pos : groundTruth) {
        if (std::find(currentQueens.begin(), currentQueens.end(), pos) == currentQueens.end()) {
            missing.push_back(pos);
        }
    }

    double correctPercent = (double)correct.size() / groundTruth.size() * 100.0;

    std::cout << "\n=== GROUND TRUTH COMPARISON ===" << std::endl;
    std::cout << "Expected queens: " << groundTruth.size() << std::endl;
    std::cout << "Placed queens: " << currentQueens.size() << std::endl;
    std::cout << "Correct positions: " << correct.size() << " / " << groundTruth.size()
              << " (" << correctPercent << "%)" << std::endl;

    bool exactMatch = (currentQueens.size() == groundTruthSet.size());
    if (exactMatch) {
        for (const auto &p : currentQueens) {
            if (groundTruthSet.find(p) == groundTruthSet.end()) {
                exactMatch = false;
                break;
            }
        }
    }

    std::cout << "Exact positional match: " << (exactMatch ? "YES" : "NO") << std::endl;

    if (!correct.empty()) {
        std::cout << "Correct: ";
        for (const auto& [r, c] : correct) {
            std::cout << "(" << r << "," << c << ") ";
        }
        std::cout << std::endl;
    }

    if (!incorrect.empty()) {
        std::cout << "Incorrect: ";
        for (const auto& [r, c] : incorrect) {
            std::cout << "(" << r << "," << c << ") ";
        }
        std::cout << std::endl;
    }

    if (!missing.empty()) {
        std::cout << "Missing: ";
        for (const auto& [r, c] : missing) {
            std::cout << "(" << r << "," << c << ") ";
        }
        std::cout << std::endl;
    }

    if (correctPercent == 100.0) {
        std::cout << "PERFECT MATCH!" << std::endl;
    }
}
