#include "../include/PuzzleSolver.h"
#include "../include/graph.h"
#include <set>
#include <map>
#include <algorithm>

const int PuzzleSolver::directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

PuzzleSolver::PuzzleSolver(Graph &graph) : puzzle(graph)
{
    // int queensPlaced = 0;
    // int backtrackCount = 0;
    // int probeCount = 0;
    // int inferredCount = 0;
}



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
    { // if there's only one unique neighbour colour then we can infer and know the exact colour
        return *neighbours.begin();
    }
    return -1;
}

int PuzzleSolver::inferWithConfidence(int row, int col)
{
    std::map<int, float> colorConfidence;

    // Neighbor inference (high confidence)
    int neighborInfer = inferNeighbours(row, col);
    if (neighborInfer != -1)
    {
        colorConfidence[neighborInfer] += 3.0;
    }

    // Row/column uniformity (medium-high confidence)
    int uniformInfer = inferRowColumnUniformity(row, col);
    if (uniformInfer != -1)
    {
        colorConfidence[uniformInfer] += 2.5;
    }

    // Domain boundaries (medium confidence)
    int domainInfer = inferFromDomains(row, col);
    if (domainInfer != -1)
    {
        colorConfidence[domainInfer] += 2.0;
    }

    // Contiguity (medium confidence)
    int contigInfer = inferFromContiguity(row, col);
    if (contigInfer != -1)
    {
        colorConfidence[contigInfer] += 2.0;
    }

    // Pattern completion (lower confidence)
    int patternInfer = inferPatternCompletion(row, col);
    if (patternInfer != -1)
    {
        colorConfidence[patternInfer] += 1.5;
    }

    // Find color with highest confidence
    int bestColor = -1;
    float maxConfidence = 4.5; // Minimum threshold (increased from 3.0 - more conservative)
                                // This makes inference harder, forcing more strategic probing

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

    // Check for L-shape patterns
    // If we have same color at (r-1,c) and (r,c-1), likely (r,c) is same
    if (row > 0 && col > 0)
    {
        int above = puzzle.getMasked()[row - 1][col];
        int left = puzzle.getMasked()[row][col - 1];
        if (above != -1 && above == left)
        {
            return above;
        }
    }

    // Check for line patterns - if colors alternate in a row/column
    // This helps identify checker patterns or stripes

    return -1;
}

int PuzzleSolver::inferRowColumnUniformity(int row, int col)
{
    int n = puzzle.getOriginal().size();

    // Check row uniformity
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

    // If row has only one color (and some unknowns), likely same color
    if (rowColors.size() == 1 && unknownCountRow > 0)
    {
        int dominantColor = rowColors.begin()->first;
        // Only infer if we have enough known cells (e.g., >50% of row)
        if (rowColors[dominantColor] >= n / 2)
        {
            return dominantColor;
        }
    }

    // Check column uniformity
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

    // Find colors whose domain contains this cell
    std::vector<int> possibleColors;
    for (auto &[color, domain] : domains)
    {
        if (domain.contains(row, col))
        {
            possibleColors.push_back(color);
        }
    }

    // If only one color's domain contains this cell
    if (possibleColors.size() == 1)
    {
        return possibleColors[0];
    }

    return -1;
}

int PuzzleSolver::inferFromContiguity(int row, int col)
{
    int n = puzzle.getOriginal().size();

    // Check if this cell bridges two regions of the same color
    std::map<int, std::vector<std::pair<int, int>>> colorRegions;

    // Collect adjacent colored cells
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

    // If we have 2+ cells of same color that aren't adjacent to each other
    // This cell likely bridges them
    for (auto &[color, cells] : colorRegions)
    {
        if (cells.size() >= 2)
        {
            // Check if these cells are disconnected without our cell
            bool disconnected = true;
            for (size_t i = 0; i < cells.size() - 1; i++)
            {
                for (size_t j = i + 1; j < cells.size(); j++)
                {
                    int dr = abs(cells[i].first - cells[j].first);
                    int dc = abs(cells[i].second - cells[j].second);
                    if (dr + dc == 1)
                    { // Adjacent to each other
                        disconnected = false;
                        break;
                    }
                }
            }

            if (disconnected)
            {
                return color; // This cell bridges disconnected regions
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

    // 1. Number of unknown neighbors (higher = more potential inferences)
    int unknownNeighbors = countUnknownNeighbors(row, col, n);
    value += unknownNeighbors * 2.0;

    // 2. Strategic position bonus (corners and edges)
    if ((row == 0 || row == n-1) && (col == 0 || col == n-1))
        value += 1.5; // Corner
    else if (row == 0 || row == n-1 || col == 0 || col == n-1)
        value += 1.0; // Edge

    // 3. Bridging potential - different colored neighbors
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
        value += neighborColors.size() * 1.5; // Bridges multiple regions

    // 4. Row criticality - prioritize rows without queens
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
        value += 2.0; // This row needs a queen

    return value;
}

void PuzzleSolver::performInferenceCascade(int n)
{
    bool madeProgress = true;
    int maxPasses = 2; // Reduced from 5 to 2 - less aggressive propagation
                       // This forces more reliance on strategic probing
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
            { // Found a queen
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

    // First, probe any unknown queen positions (minimal probing)
    for (auto [r, c] : queenPositions) {
        if (puzzle.getMasked()[r][c] == -1) {
            probe(r, c);
        }
    }

    // Now validate all color constraints
    for (size_t i = 0; i < queenPositions.size(); i++) {
        int r1 = queenPositions[i].first;
        int c1 = queenPositions[i].second;
        int color1 = puzzle.getMasked()[r1][c1];

        for (size_t j = i + 1; j < queenPositions.size(); j++) {
            int r2 = queenPositions[j].first;
            int c2 = queenPositions[j].second;
            int color2 = puzzle.getMasked()[r2][c2];

            // Check if same color
            if (color1 == color2) {
                return false; // Color constraint violated
            }
        }
    }

    return true; // All constraints satisfied
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
    int currentColour = puzzle.getMasked()[row][col]; // Get the actual color of this cell

    // CRITICAL: Cannot validate if current cell color is unknown
    if (currentColour == -1)
    {
        return false; // Must know the color before placing queen
    }

    // Check to see if the queen is in the same column
    for (int i = 0; i < row; i++)
    {
        if (puzzle.getCurrentState()[i][col] == 0)
        {
            return false;
        }
    }

    // Check diagonal adjacency ONLY (not horizontal/vertical)
    // LinkedIn Queens allows horizontal/vertical adjacency, only diagonal touching is forbidden
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < n; j++) {
            if (puzzle.getCurrentState()[i][j] == 0) { // Found a previously placed queen
                // Check if diagonally adjacent (exactly 1 step diagonal - touching corners)
                if (abs(row - i) == 1 && abs(col - j) == 1) {
                    return false; // Diagonally adjacent queens not allowed
                }
            }
        }
    }

    // Color constraint: OPTIMISTIC VALIDATION
    // Check if any previously placed queen with KNOWN color has the same color
    // Unknown colors are assumed to be different (optimistic)
    for (int i = 0; i < row; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            if (puzzle.getCurrentState()[i][j] == 0) // Found a queen
            {
                int queenColor = puzzle.getMasked()[i][j];
                // If previous queen color is known and matches, that's a conflict
                if (queenColor != -1 && queenColor == currentColour)
                {
                    return false; // Two queens in same color region
                }
                // If queenColor == -1 (unknown), optimistically assume it's different
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

    // MINIMAL SENSING: Show budget information
    if (probeBudget > 0) {
        std::cout << "\n--- Probe Budget ---" << std::endl;
        std::cout << "Initially unknown cells: " << initialUnknownCells << std::endl;
        std::cout << "Probe budget: " << probeBudget << " ("
                  << (double)probeBudget/initialUnknownCells*100 << "% of unknowns)" << std::endl;
        std::cout << "Probes used: " << probeCount << " / " << probeBudget;
        if (budgetExhausted) {
            std::cout << " ⚠️  BUDGET EXHAUSTED";
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

    // Find all queen positions
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
        std::cout << "❌ WRONG NUMBER OF QUEENS!" << std::endl;
        return;
    }

    // Check each queen
    bool hasViolations = false;
    for (auto [r, c] : queensPositions) {
        int queenColor = puzzle.getMasked()[r][c];
        std::cout << "Queen at (" << r << "," << c << ") in color " << queenColor << std::endl;

        // Verify no other queens in same color
        for (auto [r2, c2] : queensPositions) {
            if (r2 != r || c2 != c) { // Different queen
                if (puzzle.getMasked()[r2][c2] == queenColor) {
                    std::cout << "❌ VIOLATION: Multiple queens in same color " << queenColor
                             << " at (" << r << "," << c << ") and (" << r2 << "," << c2 << ")" << std::endl;
                    hasViolations = true;
                }

                // Check column constraint
                if (c2 == c) {
                    std::cout << "❌ VIOLATION: Multiple queens in same column " << c
                             << " at (" << r << "," << c << ") and (" << r2 << "," << c2 << ")" << std::endl;
                    hasViolations = true;
                }

                // Check diagonal adjacency constraint (LinkedIn Queens rule)
                // NOTE: Horizontal/vertical adjacency is ALLOWED in LinkedIn Queens
                if (abs(r2 - r) == 1 && abs(c2 - c) == 1) {
                    std::cout << "❌ VIOLATION: Queens diagonally adjacent (" << r << "," << c
                             << ") and (" << r2 << "," << c2 << ")" << std::endl;
                    hasViolations = true;
                }
            }
        }
    }

    if (hasViolations) {
        std::cout << "❌ SOLUTION HAS CONSTRAINT VIOLATIONS!" << std::endl;
    } else {
        std::cout << "✅ All constraints satisfied" << std::endl;
    }

    std::cout << "✅ Verification complete" << std::endl;
}

bool PuzzleSolver::needsProbeForDecision(int row, int col)
{
    // Only probe if:
    // 1. This position is unknown (-1)
    // 2. We need this information to place a queen or validate constraints
    // 3. We can't infer it from existing information

    if (puzzle.getMasked()[row][col] != -1) {
        return false; // Already known
    }

    // Try inference first
    int inferredColor = inferWithConfidence(row, col);
    if (inferredColor != -1) {
        puzzle.getMasked()[row][col] = inferredColor;
        inferredCount++;
        std::cout << "Inferred color " << inferredColor << " at (" << row << "," << col << ")\n";
        return false; // No probe needed, we inferred it
    }

    return true; // We need to probe this position
}

std::vector<std::pair<int, int>> PuzzleSolver::findViableQueenPositions(int row, int n)
{
    std::vector<std::pair<int, int>> viablePositions;

    for (int col = 0; col < n; col++) {
        bool basicConstraintsOK = true;

        // Check column constraint - no queen above in same column
        for (int r = 0; r < row; r++) {
            if (puzzle.getCurrentState()[r][col] == 0) {
                basicConstraintsOK = false;
                break;
            }
        }

        // Check diagonal adjacency ONLY with previous queens (LinkedIn Queens rules)
        if (basicConstraintsOK) {
            for (int r = 0; r < row; r++) {
                for (int c = 0; c < n; c++) {
                    if (puzzle.getCurrentState()[r][c] == 0) { // Found a previous queen
                        // Check diagonal adjacency (touching corners) - this is the only adjacency rule
                        if (abs(row - r) == 1 && abs(col - c) == 1) {
                            basicConstraintsOK = false;
                            break;
                        }
                    }
                }
                if (!basicConstraintsOK) break;
            }
        }

        // Check color constraint - we need to know the color before placing
        // This prevents optimistic failures that cause excessive backtracking
        if (basicConstraintsOK) {
            int cellColor = puzzle.getMasked()[row][col];

            // If color is unknown, try to infer it
            if (cellColor == -1) {
                int inferredColor = inferWithConfidence(row, col);
                if (inferredColor != -1) {
                    // Successfully inferred - update masked and check constraint
                    puzzle.getMasked()[row][col] = inferredColor;
                    inferredCount++;
                    cellColor = inferredColor;
                }
                // If still unknown after inference attempt, keep as viable
                // We'll probe it when we try to place a queen here
            }

            // If we now know the color, check if it already has a queen
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
    puzzle.getCurrentState()[row][col] = puzzle.getMasked()[row][col]; // Restore original color
    queensPlaced--;
    backtrackCount++;
    // Note: totalQueensPlaced is NOT decremented - it tracks total placement attempts
    // std::cout << "🔙 BACKTRACK: Removed queen from (" << row << "," << col << ")\n"; // Removed verbose logging
}

bool PuzzleSolver::solveRowWithBacktracking(int row, int n, std::vector<std::pair<int, int>>& queenPositions)
{
    if (row == n) {
        // Successfully placed all queens - validate final solution
        return validateFinalSolution(queenPositions);
    }

    // Track best partial solution for failed attempts
    if (queenPositions.size() > maxQueensPlaced) {
        maxQueensPlaced = queenPositions.size();
        bestPartialSolution = queenPositions;
    }

    // Find all potentially viable positions for this row
    auto viablePositions = findViableQueenPositions(row, n);

    if (viablePositions.empty()) {
        return false; // No viable positions
    }

    // Sort positions: known colors first, then by probe value (highest first)
    std::vector<std::pair<double, std::pair<int, int>>> scoredPositions;

    for (auto [r, c] : viablePositions) {
        double score = 0.0;

        if (puzzle.getMasked()[r][c] != -1) {
            // Known color - highest priority (no probe needed)
            score = 1000.0;
        } else {
            // Unknown color - try inference first
            int inferredColor = inferWithConfidence(r, c);
            if (inferredColor != -1) {
                // Can be inferred - high priority
                score = 500.0;
            } else {
                // Need to probe - score by probe value
                score = calculateProbeValue(r, c, n);
            }
        }

        scoredPositions.push_back({score, {r, c}});
    }

    // Sort by score descending (highest score first)
    std::sort(scoredPositions.begin(), scoredPositions.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });

    // Try positions in sorted order
    for (auto& [score, pos] : scoredPositions) {
        int r = pos.first;
        int c = pos.second;

        // CRITICAL: Ensure we know the color before placing a queen
        // This prevents optimistic placement failures
        if (puzzle.getMasked()[r][c] == -1) {
            // Try inference first
            int inferredColor = inferWithConfidence(r, c);
            if (inferredColor != -1) {
                puzzle.getMasked()[r][c] = inferredColor;
                inferredCount++;
            } else {
                // Must probe to know the color
                probe(r, c);
                // Run inference cascade to potentially reveal more cells
                performInferenceCascade(n);
            }
        }

        // At this point, we must know the color of (r,c)
        // Validate again now that we have color information
        int cellColor = puzzle.getMasked()[r][c];
        if (cellColor == -1) {
            // Should never happen, but safety check
            continue;
        }

        // Check if this color already has a queen
        if (hasQueenInColor(cellColor)) {
            continue; // Skip this position
        }

        // Now check all constraints with known color
        if (isValid(r, c)) {
            // Try placing queen here
            puzzle.getCurrentState()[r][c] = 0; // Place queen
            queensPlaced++;
            totalQueensPlaced++;  // Track total attempts
            queenPositions.push_back({r, c});

            // Recursively solve the next row
            if (solveRowWithBacktracking(row + 1, n, queenPositions)) {
                return true; // Found complete solution
            }

            // If placing queen here didn't lead to solution, backtrack
            undoQueenPlacement(r, c);
            queenPositions.pop_back();

            // Note: We don't undo probes - once we probe a cell, we keep that knowledge
        }
    }

    return false; // Could not place queen in this row
}

void PuzzleSolver::restoreBestPartialSolution()
{
    int n = puzzle.getOriginal().size();

    // Clear current state
    for (int r = 0; r < n; r++) {
        for (int c = 0; c < n; c++) {
            puzzle.getCurrentState()[r][c] = puzzle.getMasked()[r][c];
        }
    }

    // Restore best partial solution
    for (auto [r, c] : bestPartialSolution) {
        puzzle.getCurrentState()[r][c] = 0; // Place queen
    }
}

void PuzzleSolver::strategicSeedProbing(int n)
{
    // Strategic initial probing to seed inference system
    // Probe corners and strategic positions to maximize inference cascade

    std::vector<std::pair<int, int>> seedPositions;

    // Corners (often constrained)
    seedPositions.push_back({0, 0});
    seedPositions.push_back({0, n-1});
    seedPositions.push_back({n-1, 0});
    seedPositions.push_back({n-1, n-1});

    // Center position if board is large enough
    if (n >= 5) {
        seedPositions.push_back({n/2, n/2});
    }

    // Probe only unknown positions
    for (auto [r, c] : seedPositions) {
        if (puzzle.getMasked()[r][c] == -1) {
            probe(r, c);
        }
    }

    // Run inference cascade after seeding
    performInferenceCascade(n);
}

bool PuzzleSolver::solveWithMinimalProbing(int n)
{
    // Clear any previous state
    bestPartialSolution.clear();
    maxQueensPlaced = 0;

    // Use TRUE ACTIVE SENSING CSP approach
    bool solved = solveWithActiveCSP(n);

    // If not solved, restore the best partial solution to the board
    if (!solved && !bestPartialSolution.empty()) {
        restoreBestPartialSolution();
    }

    return solved;
}

// ============================================================================
// TRUE ACTIVE SENSING IMPLEMENTATION
// ============================================================================

double PuzzleSolver::calculateExpectedInformationGain(int row, int col, int n)
{
    // Calculate how much information we'd gain from probing this cell
    double gain = 0.0;

    // 1. Direct gain: we learn this cell's color
    gain += 1.0;

    // 2. Inference cascade potential: how many unknowns are in neighborhood?
    int unknownNeighbors = countUnknownNeighbors(row, col, n);
    gain += unknownNeighbors * 0.5; // Each unknown neighbor could be inferred

    // 3. Strategic position value
    gain += calculateProbeValue(row, col, n) * 0.1;

    // 4. Constraint propagation potential: if we probe this, how many constraints tighten?
    // Check if this cell is in a critical position for multiple colors
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
    gain += neighborColors.size() * 0.3; // More neighbor colors = more information

    return gain;
}

std::vector<std::pair<int, int>> PuzzleSolver::getMostInformativeProbes(
    int k, std::vector<std::pair<int, int>>& viablePositions)
{
    // Select k most informative cells to probe from viable positions
    int n = puzzle.getOriginal().size();
    std::vector<std::pair<double, std::pair<int, int>>> scoredProbes;

    // Score all unknown cells in viable positions and their neighborhoods
    std::set<std::pair<int, int>> candidates;

    for (auto [r, c] : viablePositions) {
        if (puzzle.getMasked()[r][c] == -1) {
            candidates.insert({r, c});
        }

        // Also consider neighbors
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

    // Score each candidate
    for (auto [r, c] : candidates) {
        double gain = calculateExpectedInformationGain(r, c, n);
        scoredProbes.push_back({gain, {r, c}});
    }

    // Sort by gain (descending)
    std::sort(scoredProbes.begin(), scoredProbes.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });

    // Return top k
    std::vector<std::pair<int, int>> result;
    for (int i = 0; i < std::min(k, (int)scoredProbes.size()); i++) {
        result.push_back(scoredProbes[i].second);
    }

    return result;
}

void PuzzleSolver::propagateConstraints(int n)
{
    // After probing/inference, propagate constraints
    // This is like AC-3 but simpler - just run inference cascade
    performInferenceCascade(n);
}

int PuzzleSolver::countViablePositions(int row, int col, int n)
{
    // Count how many positions in this cell's row/col/color are still viable
    int count = 0;

    // If color is unknown, we can't count color constraint
    int cellColor = puzzle.getMasked()[row][col];

    // Count positions in same row
    for (int c = 0; c < n; c++) {
        if (c != col) {
            auto viable = getViablePositionsForCell(row, c, n);
            count += viable.size();
        }
    }

    return count;
}

std::vector<std::pair<int, int>> PuzzleSolver::getViablePositionsForCell(int row, int col, int n)
{
    std::vector<std::pair<int, int>> viable;

    // This cell is viable if:
    // 1. No queen in same column (in previous rows)
    // 2. No diagonally adjacent queen
    // 3. Color doesn't already have queen (if known)

    int cellColor = puzzle.getMasked()[row][col];

    // Check column constraint
    bool columnOK = true;
    for (int r = 0; r < n; r++) {
        if (r != row && puzzle.getCurrentState()[r][col] == 0) {
            columnOK = false;
            break;
        }
    }

    if (!columnOK) return viable;

    // Check diagonal adjacency
    bool diagonalOK = true;
    for (int r = 0; r < n; r++) {
        for (int c = 0; c < n; c++) {
            if (puzzle.getCurrentState()[r][c] == 0) {
                if (abs(row - r) == 1 && abs(col - c) == 1) {
                    diagonalOK = false;
                    break;
                }
            }
        }
        if (!diagonalOK) break;
    }

    if (!diagonalOK) return viable;

    // Check color constraint (if color is known)
    if (cellColor != -1 && hasQueenInColor(cellColor)) {
        return viable; // Empty
    }

    viable.push_back({row, col});
    return viable;
}

std::pair<int, int> PuzzleSolver::selectMostConstrainedVariable(std::set<std::pair<int, int>>& unassigned)
{
    // MRV heuristic: select the cell with fewest viable positions
    int n = puzzle.getOriginal().size();
    std::pair<int, int> best = {-1, -1};
    int minViable = INT_MAX;

    for (auto [r, c] : unassigned) {
        // Count how many positions are viable for this row
        auto viable = findViableQueenPositions(r, n);
        if (viable.size() < minViable && viable.size() > 0) {
            minViable = viable.size();
            best = {r, c};
        }
    }

    return best;
}

bool PuzzleSolver::solveWithActiveCSP(int n)
{
    // MINIMAL SENSING: Initialize probe budget (35% of unknown cells)
    initializeProbeBudget(n, 0.50);

    // TRUE ACTIVE SENSING: Row-by-row with strategic probing BEFORE placement decisions
    std::vector<std::pair<int, int>> queenPositions;
    return solveActiveCSPBacktrack(0, n, queenPositions);
}

// Helper for recursive backtracking with active sensing
bool PuzzleSolver::solveActiveCSPBacktrack(int row, int n, std::vector<std::pair<int, int>>& queenPositions)
{
    if (row == n) {
        // All queens placed - validate solution
        return validateFinalSolution(queenPositions);
    }

    // Track progress
    if (queenPositions.size() > maxQueensPlaced) {
        maxQueensPlaced = queenPositions.size();
        bestPartialSolution = queenPositions;
    }

    // STEP 1: Get viable positions for this row
    auto viablePositions = findViableQueenPositions(row, n);

    if (viablePositions.empty()) {
        return false; // No viable positions, backtrack
    }

    // STEP 2: STRATEGIC PROBING - probe informative cells BEFORE trying placements
    // MINIMAL SENSING: Only probe if budget allows!
    if (canProbe()) {
        int maxProbesThisRound = std::min(2, (int)viablePositions.size());
        auto informativeProbes = getMostInformativeProbes(maxProbesThisRound, viablePositions);

        // BUDGET-AWARE PROBING: We're choosing which cells to probe based on information gain
        for (auto [pr, pc] : informativeProbes) {
            if (!canProbe()) break;  // Stop if budget exhausted mid-loop

            if (puzzle.getMasked()[pr][pc] == -1) {
                // Try inference first (free!)
                int inferredColor = inferWithConfidence(pr, pc);
                if (inferredColor != -1) {
                    puzzle.getMasked()[pr][pc] = inferredColor;
                    inferredCount++;
                } else {
                    // Strategic probe
                    probe(pr, pc);
                }
            }
        }
    }

    // STEP 3: Propagate constraints after probing
    propagateConstraints(n);

    // STEP 4: Re-evaluate viable positions with new information
    viablePositions = findViableQueenPositions(row, n);

    if (viablePositions.empty()) {
        return false; // Probing revealed this row is unsolvable
    }

    // STEP 5: Sort positions intelligently
    std::vector<std::pair<double, std::pair<int, int>>> scoredPositions;

    for (auto [r, c] : viablePositions) {
        double score = 0.0;

        // Prioritize known colors
        if (puzzle.getMasked()[r][c] != -1) {
            score = 1000.0;
        } else {
            // Try inference
            int inferredColor = inferWithConfidence(r, c);
            if (inferredColor != -1) {
                score = 500.0;
            } else {
                score = calculateProbeValue(r, c, n);
            }
        }

        scoredPositions.push_back({score, {r, c}});
    }

    // Sort descending by score
    std::sort(scoredPositions.begin(), scoredPositions.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });

    // STEP 6: Try placing queen in each viable position
    // MINIMAL SENSING: Use confidence-based placement when budget exhausted
    for (auto& [score, pos] : scoredPositions) {
        int r = pos.first;
        int c = pos.second;

        int cellColor = puzzle.getMasked()[r][c];

        // If unknown, try to resolve the color
        if (cellColor == -1) {
            // Try inference first
            int inferredColor = inferWithConfidence(r, c);
            if (inferredColor != -1) {
                puzzle.getMasked()[r][c] = inferredColor;
                inferredCount++;
                cellColor = inferredColor;
            } else if (canProbe()) {
                // Probe if budget allows
                probe(r, c);
                propagateConstraints(n);
                cellColor = puzzle.getMasked()[r][c];
            } else {
                // Budget exhausted - try confidence-based placement
                double confidence = 0.0;
                int predictedColor = getMostLikelyColor(r, c, confidence);

                // Use prediction if confidence is high enough (threshold: 2.0 - lower for more attempts)
                if (confidence >= 2.0 && predictedColor != -1) {
                    cellColor = predictedColor;
                    // Don't update masked matrix - this is a tentative guess
                    // We'll backtrack if it fails
                } else {
                    // Not confident enough - skip this position
                    continue;
                }
            }
        }

        if (cellColor == -1) continue;  // Still unknown, skip

        // Check color constraint
        if (hasQueenInColor(cellColor)) continue;

        // Validate all constraints
        if (isValid(r, c)) {
            // Place queen
            puzzle.getCurrentState()[r][c] = 0;
            queensPlaced++;
            totalQueensPlaced++;
            queenPositions.push_back({r, c});

            // Recursively solve next row
            if (solveActiveCSPBacktrack(row + 1, n, queenPositions)) {
                return true; // Solution found!
            }

            // Backtrack
            undoQueenPlacement(r, c);
            queenPositions.pop_back();
        }
    }

    return false; // No solution from this configuration
}

// ==================== MINIMAL SENSING: PROBE BUDGET SYSTEM ====================

// Initialize probe budget as a percentage of initially unknown cells
void PuzzleSolver::initializeProbeBudget(int n, double budgetPercent)
{
    // Count initially unknown cells
    initialUnknownCells = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (puzzle.getMasked()[i][j] == -1) {
                initialUnknownCells++;
            }
        }
    }

    // Set budget as percentage of unknown cells
    probeBudget = static_cast<int>(initialUnknownCells * budgetPercent);
    budgetExhausted = false;

    std::cout << "[MINIMAL SENSING] Probe budget initialized:\n";
    std::cout << "  Unknown cells: " << initialUnknownCells << "\n";
    std::cout << "  Budget: " << probeBudget << " probes ("
              << (budgetPercent * 100) << "% of unknowns)\n";
}

// Check if we can still probe (budget remaining)
bool PuzzleSolver::canProbe()
{
    if (probeCount >= probeBudget) {
        budgetExhausted = true;
        return false;
    }
    return true;
}

// Get most likely color for a cell with confidence score
int PuzzleSolver::getMostLikelyColor(int row, int col, double& confidence)
{
    // This uses the same logic as inferWithConfidence but returns confidence
    std::map<int, float> colorConfidence;
    int n = puzzle.getSize();

    // Try all inference techniques and aggregate scores
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

    // Find color with highest confidence
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

// Try to place queen with incomplete information (confidence-based)
bool PuzzleSolver::tryPlaceWithConfidence(int row, int col, double minConfidence)
{
    double confidence = 0.0;
    int predictedColor = getMostLikelyColor(row, col, confidence);

    if (confidence >= minConfidence && predictedColor != -1) {
        // Use predicted color without probing
        puzzle.getMasked()[row][col] = predictedColor;
        inferredCount++;
        return true;
    }

    return false;  // Not confident enough
}
