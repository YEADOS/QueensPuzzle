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

bool PuzzleSolver::solvePuzzle(int row, int n)
{
    if (row == n)
    {
        return true;
    }

    // IMPROVEMENT 1: Do a full inference pass before attempting to place queens
    // This maximizes inference and minimizes probing
    performFullInference(n);

    for (int col = 0; col < n; ++col)
    {
        // IMPROVEMENT 2: Global Active Sensing Strategy
        if (puzzle.getMasked()[row][col] == -1)
        {
            // Try one more targeted inference for this specific cell
            int inferredColour = inferWithConfidence(row, col);

            if (inferredColour != -1)
            {
                puzzle.getMasked()[row][col] = inferredColour;
                inferredCount++;
                std::cout << "Inferred color " << inferredColour
                          << " at (" << row << "," << col << ")\n";
            }
            else
            {
                // Local active sensing - simulate nearby options and pick best
                auto bestLocalProbe = selectBestLocalProbe(row, col, n);
                if (bestLocalProbe.first != -1 && bestLocalProbe.second != -1) {
                    std::cout << "LOCAL ACTIVE SENSING: Choosing (" << bestLocalProbe.first
                             << "," << bestLocalProbe.second << ") over current (" << row << "," << col << ")\n";
                    probe(bestLocalProbe.first, bestLocalProbe.second);
                } else {
                    // Fallback to current position
                    probe(row, col);
                }
            }
        }

        if (isValid(row, col))
        {
            int originalColor = puzzle.getMasked()[row][col];
            puzzle.getCurrentState()[row][col] = 0; // Place queen
            queensPlaced++;

            if (solvePuzzle(row + 1, n))
                return true;

            puzzle.getCurrentState()[row][col] = originalColor; // Backtrack
            backtrackCount++;
        }
    }
    return false;
}

void PuzzleSolver::performFullInference(int n)
{
    bool madeInference;
    int passes = 0;

    do
    {
        madeInference = false;
        passes++;

        for (int r = 0; r < n; r++)
        {
            for (int c = 0; c < n; c++)
            {
                if (puzzle.getMasked()[r][c] == -1)
                {
                    // Try multiple inference strategies
                    int inferredColor = inferWithConfidence(r, c);

                    if (inferredColor != -1)
                    {
                        puzzle.getMasked()[r][c] = inferredColor;
                        inferredCount++;
                        madeInference = true;
                        std::cout << "Pass " << passes << ": Inferred color "
                                  << inferredColor << " at (" << r << "," << c << ")\n";
                    }
                }
            }
        }
    } while (madeInference && passes < 10); // Limit passes to avoid infinite loops
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
        std::cout << "Fired\n";
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
    float maxConfidence = 3.0; // Minimum threshold

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

void PuzzleSolver::probe(int row, int col)
{
    probeCount++;
    int colour = puzzle.getOriginal()[row][col];
    std::cout << "Probed color " << colour << " at (" << row << "," << col << ")\n";
    puzzle.getMasked()[row][col] = colour;
}

bool PuzzleSolver::isValid(int row, int col)
{

    int n = puzzle.getOriginal().size();
    int currentColour = puzzle.getMasked()[row][col]; // Get the actual color of this cell

    // Check to see if the queen is in the same column
    for (int i = 0; i < row; i++)
    {
        if (puzzle.getCurrentState()[i][col] == 0)
        {
            return false;
        }
    }

    // Check all diagonal adjacency with ALL previously placed queens
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < n; j++) {
            if (puzzle.getCurrentState()[i][j] == 0) { // Found a previously placed queen
                // Check if diagonally adjacent (any diagonal direction)
                if (abs(row - i) == 1 && abs(col - j) == 1) {
                    return false; // Diagonally adjacent queens not allowed
                }

                // Also check if horizontally or vertically adjacent
                if ((abs(row - i) == 1 && col == j) || (row == i && abs(col - j) == 1)) {
                    return false; // Adjacent queens not allowed
                }
            }
        }
    }

    for (int i = 0; i < row; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            if (puzzle.getCurrentState()[i][j] == 0 && puzzle.getMasked()[i][j] == currentColour) // add condition to check if there's a -1 meaning it is unsure of if its correct
                return false;
        }
    }
    return true;
}

double PuzzleSolver::calculateProbeValue(int row, int col)
{
    if (puzzle.getMasked()[row][col] != -1) {
        return 0.0; // Already known
    }

    double value = 0.0;
    int n = puzzle.getOriginal().size();

    // Factor 1: Number of unknown neighbors (higher = more valuable)
    int unknownNeighbors = 0;
    int knownNeighbors = 0;

    for (int i = 0; i < 4; i++) {
        int nr = row + directions[i][0];
        int nc = col + directions[i][1];

        if (nr >= 0 && nr < n && nc >= 0 && nc < n) {
            if (puzzle.getMasked()[nr][nc] == -1) {
                unknownNeighbors++;
            } else {
                knownNeighbors++;
            }
        }
    }

    // Higher value if this probe would help infer many neighbors
    value += unknownNeighbors * 2.0;

    // Factor 2: Strategic position (corners and edges are often easier to infer)
    if ((row == 0 || row == n-1) && (col == 0 || col == n-1)) {
        value += 1.5; // Corner
    } else if (row == 0 || row == n-1 || col == 0 || col == n-1) {
        value += 1.0; // Edge
    }

    // Factor 3: Would this probe bridge disconnected regions?
    std::map<int, int> neighborColors;
    for (int i = 0; i < 4; i++) {
        int nr = row + directions[i][0];
        int nc = col + directions[i][1];

        if (nr >= 0 && nr < n && nc >= 0 && nc < n) {
            int color = puzzle.getMasked()[nr][nc];
            if (color != -1) {
                neighborColors[color]++;
            }
        }
    }

    // If multiple different colors adjacent, this might be a bridge point
    if (neighborColors.size() >= 2) {
        value += 3.0;
    }

    // Factor 4: Would help with queen placement constraints?
    // Check if this is in a critical row for queen placement
    bool criticalForQueens = false;
    int queensInRow = 0;
    for (int c = 0; c < n; c++) {
        if (puzzle.getCurrentState()[row][c] == 0) {
            queensInRow++;
        }
    }

    if (queensInRow == 0) { // No queen placed in this row yet
        value += 1.5;
    }

    return value;
}

std::pair<int, int> PuzzleSolver::selectBestProbeTarget(int n)
{
    double bestValue = -1.0;
    std::pair<int, int> bestTarget = {-1, -1};

    // Evaluate all unknown positions
    for (int r = 0; r < n; r++) {
        for (int c = 0; c < n; c++) {
            if (puzzle.getMasked()[r][c] == -1) {
                double value = calculateProbeValue(r, c);
                if (value > bestValue) {
                    bestValue = value;
                    bestTarget = {r, c};
                }
            }
        }
    }

    // If we found a valuable target, use it
    if (bestValue > 2.0) { // Threshold for "worth probing"
        std::cout << "Smart probe selection: (" << bestTarget.first
                 << "," << bestTarget.second << ") with value " << bestValue << "\n";
        return bestTarget;
    }

    return {-1, -1}; // No good probe target found
}

bool PuzzleSolver::shouldSkipCurrentPosition(int currentRow, int currentCol, int n)
{
    // Calculate value of current position
    double currentValue = calculateProbeValue(currentRow, currentCol);

    // Find the globally best probe position
    auto globalBest = selectGlobalBestProbe(currentRow, currentCol, n);

    if (globalBest.first == -1) {
        return false; // No alternative found
    }

    double globalBestValue = calculateProbeValue(globalBest.first, globalBest.second);

    // Skip current position if global best is significantly better
    bool shouldSkip = globalBestValue > (currentValue * skipThreshold);

    if (shouldSkip) {
        std::cout << "SKIP DECISION: Current (" << currentRow << "," << currentCol
                 << ") value=" << currentValue
                 << " vs Global best (" << globalBest.first << "," << globalBest.second
                 << ") value=" << globalBestValue << " (threshold=" << skipThreshold << ")\n";
    }

    return shouldSkip;
}

std::pair<int, int> PuzzleSolver::selectGlobalBestProbe(int currentRow, int currentCol, int n)
{
    double bestValue = -1.0;
    std::pair<int, int> bestTarget = {-1, -1};

    // Evaluate ALL unknown positions globally (not just current row/col vicinity)
    for (int r = 0; r < n; r++) {
        for (int c = 0; c < n; c++) {
            if (puzzle.getMasked()[r][c] == -1) {
                double value = calculateProbeValue(r, c);

                // Add bonus for positions that would unlock many inferences
                // (simulate lookahead: what could we infer after this probe?)
                double lookaheadBonus = 0.0;

                // Count how many unknown neighbors this position has
                int unknownNeighborsOfNeighbors = 0;
                for (int i = 0; i < 4; i++) {
                    int nr = r + directions[i][0];
                    int nc = c + directions[i][1];

                    if (nr >= 0 && nr < n && nc >= 0 && nc < n && puzzle.getMasked()[nr][nc] == -1) {
                        // Count neighbors of this neighbor (cascade effect)
                        for (int j = 0; j < 4; j++) {
                            int nnr = nr + directions[j][0];
                            int nnc = nc + directions[j][1];

                            if (nnr >= 0 && nnr < n && nnc >= 0 && nnc < n &&
                                puzzle.getMasked()[nnr][nnc] == -1) {
                                unknownNeighborsOfNeighbors++;
                            }
                        }
                    }
                }

                // Bonus for positions that could trigger inference cascades
                lookaheadBonus = unknownNeighborsOfNeighbors * 0.5;
                value += lookaheadBonus;

                if (value > bestValue) {
                    bestValue = value;
                    bestTarget = {r, c};
                }
            }
        }
    }

    return bestTarget;
}


std::vector<std::pair<int, int>> PuzzleSolver::getLocalProbeCandidate(int currentRow, int currentCol, int n)
{
    std::vector<std::pair<int, int>> candidates;

    // Always include current position as baseline
    if (puzzle.getMasked()[currentRow][currentCol] == -1) {
        candidates.push_back({currentRow, currentCol});
    }

    // Add nearby unknown positions (within 2-cell radius)
    int radius = 2;
    for (int dr = -radius; dr <= radius; dr++) {
        for (int dc = -radius; dc <= radius; dc++) {
            int r = currentRow + dr;
            int c = currentCol + dc;

            if (r >= 0 && r < n && c >= 0 && c < n &&
                puzzle.getMasked()[r][c] == -1 &&
                !(r == currentRow && c == currentCol)) {

                candidates.push_back({r, c});
            }
        }
    }

    // Limit to top 4 candidates to keep simulation manageable
    if (candidates.size() > 4) {
        // Sort by distance from current position (closer is better for local sensing)
        std::sort(candidates.begin(), candidates.end(),
                 [currentRow, currentCol](const std::pair<int, int>& a, const std::pair<int, int>& b) {
                     int distA = abs(a.first - currentRow) + abs(a.second - currentCol);
                     int distB = abs(b.first - currentRow) + abs(b.second - currentCol);
                     return distA < distB;
                 });
        candidates.resize(4);
    }

    return candidates;
}

int PuzzleSolver::simulateProbeOutcome(int row, int col, int n)
{
    // Create a temporary copy of the current masked state
    auto originalMasked = puzzle.getMasked();

    // Simulate the probe
    int actualColor = puzzle.getOriginal()[row][col];
    puzzle.getMasked()[row][col] = actualColor;

    // Count how many new inferences this probe enables
    int newInferences = 0;
    for (int r = 0; r < n; r++) {
        for (int c = 0; c < n; c++) {
            if (puzzle.getMasked()[r][c] == -1) {
                int inferredColor = inferWithConfidence(r, c);
                if (inferredColor != -1) {
                    newInferences++;
                }
            }
        }
    }

    // Additional scoring factors
    int score = newInferences * 10; // Base score from enabling inferences

    // Bonus for positions that help with current row queen placement
    bool helpsCurrentRow = true; // This probe is helping us make progress
    if (helpsCurrentRow) {
        score += 5;
    }

    // Bonus for strategic positions (corners, edges)
    if ((row == 0 || row == n-1) && (col == 0 || col == n-1)) {
        score += 3; // Corner
    } else if (row == 0 || row == n-1 || col == 0 || col == n-1) {
        score += 2; // Edge
    }

    // Restore original state
    puzzle.getMasked() = originalMasked;

    return score;
}

std::pair<int, int> PuzzleSolver::selectBestLocalProbe(int currentRow, int currentCol, int n)
{
    auto candidates = getLocalProbeCandidate(currentRow, currentCol, n);

    if (candidates.empty()) {
        return {-1, -1}; // No candidates
    }

    int bestScore = -1;
    std::pair<int, int> bestCandidate = {-1, -1};

    std::cout << "Simulating " << candidates.size() << " local probe candidates around ("
              << currentRow << "," << currentCol << "):\n";

    for (auto [r, c] : candidates) {
        int score = simulateProbeOutcome(r, c, n);

        std::cout << "  Candidate (" << r << "," << c << ") -> score: " << score << std::endl;

        if (score > bestScore) {
            bestScore = score;
            bestCandidate = {r, c};
        }
    }

    // Only choose an alternative if it's significantly better than current position
    if (bestCandidate.first == currentRow && bestCandidate.second == currentCol) {
        return {-1, -1}; // Current position is already best, no need to specify
    }

    // Require at least 20% better score to switch from current position
    int currentScore = simulateProbeOutcome(currentRow, currentCol, n);
    if (bestScore > currentScore * 1.2) {
        std::cout << "Selecting (" << bestCandidate.first << "," << bestCandidate.second
                 << ") with score " << bestScore << " over current score " << currentScore << std::endl;
        return bestCandidate;
    }

    return {-1, -1}; // Current position is good enough
}

void PuzzleSolver::printStatistics()
{
    std::cout << "\n=== GLOBAL ACTIVE SENSING Statistics ===" << std::endl;
    std::cout << "Queen placements: " << queensPlaced << std::endl;
    std::cout << "Backtracks: " << backtrackCount << std::endl;
    std::cout << "Probe operations: " << probeCount << std::endl;
    std::cout << "Inferred operations: " << inferredCount << std::endl;
    std::cout << "Sensing budget used: " << (15 - sensingBudget) << "/15" << std::endl;
    std::cout << "Remaining budget: " << sensingBudget << std::endl;

    // Calculate efficiency metrics
    double efficiency = (double)queensPlaced / (queensPlaced + backtrackCount + 1);
    double inferenceRatio = (double)inferredCount / (inferredCount + probeCount + 1);
    double budgetEfficiency = (double)probeCount / 15.0; // How much of budget was used

    std::cout << "Search efficiency: " << efficiency << std::endl;
    std::cout << "Inference vs Probe ratio: " << inferenceRatio << std::endl;
    std::cout << "Budget efficiency: " << budgetEfficiency << " (lower is better)" << std::endl;
    std::cout << "Total operations: " << (queensPlaced + backtrackCount + probeCount + inferredCount) << std::endl;
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

                // Check diagonal constraint (must be exactly 1 step diagonally)
                if (abs(r2 - r) == 1 && abs(c2 - c) == 1) {
                    std::cout << "❌ VIOLATION: Queens diagonally adjacent (" << r << "," << c
                             << ") and (" << r2 << "," << c2 << ")" << std::endl;
                    hasViolations = true;
                }

                // Check horizontally/vertically adjacent constraint
                if ((abs(r2 - r) == 1 && c2 == c) || (r2 == r && abs(c2 - c) == 1)) {
                    std::cout << "❌ VIOLATION: Queens horizontally/vertically adjacent (" << r << "," << c
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
        // Quick constraint check without knowing the color
        bool basicConstraintsOK = true;

        // Check column constraint
        for (int r = 0; r < row; r++) {
            if (puzzle.getCurrentState()[r][col] == 0) {
                basicConstraintsOK = false;
                break;
            }
        }

        // Check diagonal constraints
        if (basicConstraintsOK && row > 0) {
            if (col > 0 && puzzle.getCurrentState()[row - 1][col - 1] == 0)
                basicConstraintsOK = false;
            if (col < n - 1 && puzzle.getCurrentState()[row - 1][col + 1] == 0)
                basicConstraintsOK = false;
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
    // std::cout << "🔙 BACKTRACK: Removed queen from (" << row << "," << col << ")\n"; // Removed verbose logging
}

bool PuzzleSolver::solveRowWithBacktracking(int row, int n, std::vector<std::pair<int, int>>& queenPositions)
{
    if (row == n) {
        return true; // Successfully placed all queens
    }

    // std::cout << "\n--- SOLVING ROW " << row << " ---\n"; // Removed verbose logging

    // Find all potentially viable positions for this row
    auto viablePositions = findViableQueenPositions(row, n);

    // std::cout << "Found " << viablePositions.size() << " potentially viable positions in row " << row << "\n"; // Removed verbose logging

    for (auto [r, c] : viablePositions) {
        // std::cout << "Trying position (" << r << "," << c << "):\n"; // Reduce verbosity

        // Check if we need to probe this position to make a decision
        if (needsProbeForDecision(r, c)) {
            std::cout << "PROBE: (" << r << "," << c << ") color " << puzzle.getOriginal()[r][c] << "\n";
            probe(r, c);
        }

        // Now check if this position is valid for queen placement
        if (isValid(r, c)) {
            // Try placing queen here
            puzzle.getCurrentState()[r][c] = 0; // Place queen
            queensPlaced++;
            queenPositions.push_back({r, c});

            // After placing a queen, do minimal targeted inference
            // performFullInference(n); // This was revealing too much - comment out

            // Recursively solve the next row
            if (solveRowWithBacktracking(row + 1, n, queenPositions)) {
                return true; // Found complete solution
            }

            // If placing queen here didn't lead to solution, backtrack
            undoQueenPlacement(r, c);
            queenPositions.pop_back();

            // Note: We don't undo probes - once we probe a cell, we keep that knowledge
            // This maintains active sensing efficiency
        }
    }

    // std::cout << "❌ No valid queen placement found for row " << row << "\n"; // Removed verbose logging
    return false; // Could not place queen in this row
}

bool PuzzleSolver::solveWithMinimalProbing(int n)
{
    // Skip aggressive inference - only probe what we actually need
    // performFullInference(n); // This was revealing everything

    // std::cout << "🧠 Starting minimal probing solver with backtracking\n"; // Removed verbose logging

    std::vector<std::pair<int, int>> queenPositions; // Track queen positions for backtracking

    bool solved = solveRowWithBacktracking(0, n, queenPositions);

    if (solved) {
        std::cout << "SOLUTION FOUND! Queens at: ";
        for (auto [r, c] : queenPositions) {
            std::cout << "(" << r << "," << c << ") ";
        }
        std::cout << "\n";
    }

    return solved;
}
