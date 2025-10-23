# Program Flow Documentation

## Overview
This document details the complete execution flow of the QueensPuzzle solver from program start to finish, focusing on the **currently active implementation** using `solveWithMinimalProbing()`.

---

## 1. Program Entry Point: `main()` (src/main.cpp)

### Initial Setup
```cpp
int numPuzzles = 20;
std::string puzzleFileName = "puzzles.txt";
std::vector<Graph> graphs;
```

**Key Variables:**
- `numPuzzles`: Number of puzzles to load from file (20)
- `puzzleFileName`: Input file containing puzzle data ("puzzles.txt")
- `graphs`: Vector to store loaded Graph objects

### Step 1.1: Load Puzzles
```cpp
PuzzleManager::loadFromFile(puzzleFileName, numPuzzles, graphs);
```

**Calls:** `PuzzleManager::loadFromFile()` → See Section 2

---

## 2. Puzzle Loading: `PuzzleManager::loadFromFile()` (src/PuzzleManager.cpp)

### Purpose
Reads puzzle data from text file and creates Graph objects.

### Process Flow

**Step 2.1:** Open file
```cpp
std::ifstream puzzleFile(filename);
```

**Step 2.2:** Loop through each puzzle (20 times)
```cpp
for (int i = 0; i < numPuzzles; i++)
```

**Step 2.3:** Read puzzle size
```cpp
puzzleFile >> graphSize;  // First number in each puzzle block
```

**Step 2.4:** Read puzzle matrix data
```cpp
std::vector<std::vector<int>> puzzleData(graphSize, std::vector<int>(graphSize));
for (int row = 0; row < graphSize; row++) {
    for (int col = 0; col < graphSize; col++) {
        puzzleFile >> puzzleData[row][col];  // Read color values
    }
}
```

**Step 2.5:** Create Graph and add to vector
```cpp
Graph newGraph(puzzleData);
graphs.push_back(newGraph);
```

**Calls:** `Graph` constructor → See Section 3

---

## 3. Graph Construction: `Graph::Graph()` (src/graph.cpp)

### Constructor Signature
```cpp
Graph::Graph(const std::vector<std::vector<int>>& data)
    : original(data), currentState(data), masked(createMaskedMatrix(data, 0.2))
```

### Key Member Variables Initialized

**`original`** (const)
- Complete puzzle with all colors known
- Never modified during solving
- Used as ground truth for probing

**`currentState`**
- Working copy initialized from `original`
- Modified during solving (queens placed as `0`)
- Represents current state of queen placements

**`masked`**
- Partially revealed board
- Created by `createMaskedMatrix(data, 0.2)`
- Unknown cells marked as `-1`
- 20% of cells randomly masked

### Step 3.1: Create Masked Matrix
**Calls:** `createMaskedMatrix(original, 0.2)` → See Section 3.1

---

## 3.1 Masking Process: `Graph::createMaskedMatrix()` (src/graph.cpp)

### Purpose
Randomly mask 20% of board cells to simulate partial information.

### Process
```cpp
std::vector<std::vector<int>> masked = original;
std::random_device rd;
std::mt19937 gen(rd());
std::bernoulli_distribution mask(mask_prob);  // mask_prob = 0.2

for (size_t i = 0; i < masked.size(); ++i) {
    for (size_t j = 0; j < masked[i].size(); ++j) {
        if (mask(gen)) {
            masked[i][j] = -1;  // Mark as unknown
        }
    }
}
return masked;
```

**Key Points:**
- 20% probability each cell becomes `-1` (masked)
- 80% of cells remain visible initially
- Randomized using Bernoulli distribution

---

## 4. Main Solving Loop (back in main.cpp)

### Loop Through All Puzzles
```cpp
for (auto &g : graphs)
{
    std::cout << "\n===== PUZZLE " << puzzleNumber << "/" << numPuzzles << " =====" << std::endl;

    PuzzleSolver solver(g);
    g.printGraph(g.MASKED);  // Show initial masked state

    if (!solver.solveWithMinimalProbing(g.getSize())) {
        std::cout << "❌ PUZZLE " << puzzleNumber << " - No solution found.\n";
    } else {
        std::cout << "✅ PUZZLE " << puzzleNumber << " - Solution found!\n";
        solvedCount++;

        // Print results
        g.printGraph(g.ORIGINAL);
        g.printGraph(g.CURRENT_SYMBOLS);
        g.printGraph(g.MASKED);

        solver.verifyQueenPlacement();
    }
    solver.printStatistics();
    puzzleNumber++;
}
```

### Step 4.1: Create PuzzleSolver
**Calls:** `PuzzleSolver::PuzzleSolver(Graph &graph)` → See Section 5

### Step 4.2: Solve Puzzle
**Calls:** `solver.solveWithMinimalProbing(n)` → See Section 6

---

## 5. PuzzleSolver Construction (src/PuzzleSolver.cpp)

### Constructor
```cpp
PuzzleSolver::PuzzleSolver(Graph &graph) : puzzle(graph)
{
    // Member variables initialized with default values:
    // queensPlaced = 0
    // backtrackCount = 0
    // probeCount = 0
    // inferredCount = 0
    // sensingBudget = 15 (NOT enforced currently)
    // skipThreshold = 1.5 (NOT used in current flow)
}
```

### Key Member Variables

**Statistics Tracking:**
- `queensPlaced`: Count of queens successfully placed
- `backtrackCount`: Number of backtrack operations
- `probeCount`: Number of cells actively probed/revealed
- `inferredCount`: Number of cells deduced via inference

**Reference:**
- `puzzle`: Reference to the Graph being solved

**Constants:**
- `directions[4][2]`: {{-1,0}, {1,0}, {0,-1}, {0,1}} (up, down, left, right)

---

## 6. Main Solving Algorithm: `solveWithMinimalProbing()` (src/PuzzleSolver.cpp:824)

### Entry Point
```cpp
bool PuzzleSolver::solveWithMinimalProbing(int n)
{
    std::vector<std::pair<int, int>> queenPositions;
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
```

**Key Variables:**
- `queenPositions`: Tracks all placed queen positions for output
- `solved`: Boolean result from backtracking

**Calls:** `solveRowWithBacktracking(0, n, queenPositions)` → See Section 7

**Note:** This function does NOT call `performFullInference()` upfront (commented out line 827)

---

## 7. Core Backtracking: `solveRowWithBacktracking()` (src/PuzzleSolver.cpp:774)

### Purpose
Recursively place queens row-by-row using backtracking with minimal probing.

### Function Signature
```cpp
bool PuzzleSolver::solveRowWithBacktracking(int row, int n, std::vector<std::pair<int, int>>& queenPositions)
```

### Parameters
- `row`: Current row being solved (0-indexed)
- `n`: Board size (NxN)
- `queenPositions`: Reference to vector tracking queen placements

---

### STEP 7.1: Base Case - Success
```cpp
if (row == n) {
    return true; // Successfully placed all queens
}
```

If we've placed queens in all N rows, puzzle is solved.

---

### STEP 7.2: Find Viable Positions
```cpp
auto viablePositions = findViableQueenPositions(row, n);
```

**Calls:** `findViableQueenPositions(row, n)` → See Section 7.2

**Returns:** Vector of `(row, col)` pairs that pass basic constraint checks

---

### STEP 7.3: Try Each Viable Position
```cpp
for (auto [r, c] : viablePositions) {
    // Step 7.3.1: Check if probe needed
    if (needsProbeForDecision(r, c)) {
        std::cout << "PROBE: (" << r << "," << c << ") color " << puzzle.getOriginal()[r][c] << "\n";
        probe(r, c);
    }

    // Step 7.3.2: Validate position
    if (isValid(r, c)) {
        // Step 7.3.3: Place queen
        puzzle.getCurrentState()[r][c] = 0;
        queensPlaced++;
        queenPositions.push_back({r, c});

        // Step 7.3.4: Recurse to next row
        if (solveRowWithBacktracking(row + 1, n, queenPositions)) {
            return true; // Found complete solution
        }

        // Step 7.3.5: Backtrack
        undoQueenPlacement(r, c);
        queenPositions.pop_back();
    }
}

return false; // Could not place queen in this row
```

---

## 7.2 Finding Viable Positions: `findViableQueenPositions()` (src/PuzzleSolver.cpp:734)

### Purpose
Pre-filter positions that violate basic constraints (without knowing colors).

### Process Flow

```cpp
std::vector<std::pair<int, int>> viablePositions;

for (int col = 0; col < n; col++) {
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
```

### Constraints Checked
1. **Column uniqueness:** No queen already in this column
2. **Diagonal adjacency:** No queen diagonally adjacent in previous row

**Note:** This does NOT check color constraints (those require knowing cell colors)

---

## 7.3 Probe Decision: `needsProbeForDecision()` (src/PuzzleSolver.cpp:711)

### Purpose
Determine if a cell needs to be probed or can be inferred.

### Process Flow

```cpp
bool PuzzleSolver::needsProbeForDecision(int row, int col)
{
    // Already known?
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
```

**Decision Tree:**
1. If cell is already known (`!= -1`): **Don't probe**
2. If inference succeeds: **Don't probe** (use inferred value)
3. Otherwise: **Probe required**

**Calls:** `inferWithConfidence(row, col)` → See Section 8

---

## 8. Inference System: `inferWithConfidence()` (src/PuzzleSolver.cpp:137)

### Purpose
Attempt to deduce cell color using multiple heuristic strategies with confidence scoring.

### Confidence-Based Approach

```cpp
int PuzzleSolver::inferWithConfidence(int row, int col)
{
    std::map<int, float> colorConfidence;

    // Strategy 1: Neighbor inference (high confidence: 3.0)
    int neighborInfer = inferNeighbours(row, col);
    if (neighborInfer != -1) {
        colorConfidence[neighborInfer] += 3.0;
    }

    // Strategy 2: Row/column uniformity (medium-high: 2.5)
    int uniformInfer = inferRowColumnUniformity(row, col);
    if (uniformInfer != -1) {
        colorConfidence[uniformInfer] += 2.5;
    }

    // Strategy 3: Domain boundaries (medium: 2.0)
    int domainInfer = inferFromDomains(row, col);
    if (domainInfer != -1) {
        colorConfidence[domainInfer] += 2.0;
    }

    // Strategy 4: Contiguity (medium: 2.0)
    int contigInfer = inferFromContiguity(row, col);
    if (contigInfer != -1) {
        colorConfidence[contigInfer] += 2.0;
    }

    // Strategy 5: Pattern completion (lower: 1.5)
    int patternInfer = inferPatternCompletion(row, col);
    if (patternInfer != -1) {
        colorConfidence[patternInfer] += 1.5;
    }

    // Find color with highest confidence
    int bestColor = -1;
    float maxConfidence = 3.0; // Minimum threshold

    for (auto &[color, confidence] : colorConfidence) {
        if (confidence > maxConfidence) {
            maxConfidence = confidence;
            bestColor = color;
        }
    }

    return bestColor; // -1 if no inference meets threshold
}
```

### Inference Strategies (by priority)

**Each strategy is detailed in Sections 8.1 - 8.5**

---

## 8.1 Neighbor Inference: `inferNeighbours()` (src/PuzzleSolver.cpp:107)

### Confidence Weight: 3.0 (Highest)

### Logic
If ALL 4-directional neighbors are known AND have the same color → infer center cell is same color.

### Implementation
```cpp
int PuzzleSolver::inferNeighbours(int row, int col)
{
    int n = puzzle.getOriginal().size();
    std::set<int> neighbours;

    for (int i = 0; i < 4; i++) {
        int newRow = row + directions[i][0];
        int newCol = col + directions[i][1];

        if (newRow >= 0 && newRow < n && newCol >= 0 && newCol < n) {
            int neighbourColour = puzzle.getMasked()[newRow][newCol];
            if (neighbourColour != -1) {
                neighbours.insert(neighbourColour);
            } else {
                return -1; // Unknown neighbor - can't infer
            }
        }
    }

    if (neighbours.size() == 1) {
        return *neighbours.begin(); // All neighbors same color
    }
    return -1;
}
```

**Key Points:**
- Uses `directions[4][2]` for up/down/left/right
- Requires ALL valid neighbors to be known
- Requires ALL neighbors to be same color
- Returns `-1` if any neighbor unknown or multiple colors exist

---

## 8.2 Row/Column Uniformity: `inferRowColumnUniformity()` (src/PuzzleSolver.cpp:215)

### Confidence Weight: 2.5

### Logic
If a row/column has only ONE color visible (and unknowns), and that color appears in ≥50% of cells → infer unknowns are same color.

### Implementation
```cpp
int PuzzleSolver::inferRowColumnUniformity(int row, int col)
{
    int n = puzzle.getOriginal().size();

    // Check row uniformity
    std::map<int, int> rowColors;
    int unknownCountRow = 0;
    for (int c = 0; c < n; c++) {
        int color = puzzle.getMasked()[row][c];
        if (color == -1)
            unknownCountRow++;
        else
            rowColors[color]++;
    }

    if (rowColors.size() == 1 && unknownCountRow > 0) {
        int dominantColor = rowColors.begin()->first;
        if (rowColors[dominantColor] >= n / 2) {
            return dominantColor;
        }
    }

    // Check column uniformity (same logic)
    std::map<int, int> colColors;
    int unknownCountCol = 0;
    for (int r = 0; r < n; r++) {
        int color = puzzle.getMasked()[r][col];
        if (color == -1)
            unknownCountCol++;
        else
            colColors[color]++;
    }

    if (colColors.size() == 1 && unknownCountCol > 0) {
        int dominantColor = colColors.begin()->first;
        if (colColors[dominantColor] >= n / 2) {
            return dominantColor;
        }
    }

    return -1;
}
```

**Requirements:**
- Only ONE distinct color in row/column
- That color appears in ≥50% of cells
- Some cells are unknown

---

## 8.3 Domain Inference: `inferFromDomains()` (src/PuzzleSolver.cpp:286)

### Confidence Weight: 2.0

### Logic
Compute bounding box (domain) for each color. If only ONE color's domain contains the target cell → infer that color.

### Implementation
```cpp
int PuzzleSolver::inferFromDomains(int row, int col)
{
    std::map<int, ColorDomain> domains;
    computeColorDomains(domains);

    // Find colors whose domain contains this cell
    std::vector<int> possibleColors;
    for (auto &[color, domain] : domains) {
        if (domain.contains(row, col)) {
            possibleColors.push_back(color);
        }
    }

    if (possibleColors.size() == 1) {
        return possibleColors[0];
    }

    return -1;
}
```

### Helper: `computeColorDomains()`
```cpp
void PuzzleSolver::computeColorDomains(std::map<int, ColorDomain> &domains)
{
    int n = puzzle.getOriginal().size();

    for (int r = 0; r < n; r++) {
        for (int c = 0; c < n; c++) {
            int color = puzzle.getMasked()[r][c];
            if (color != -1) {
                domains[color].minRow = std::min(domains[color].minRow, r);
                domains[color].maxRow = std::max(domains[color].maxRow, r);
                domains[color].minCol = std::min(domains[color].minCol, c);
                domains[color].maxCol = std::max(domains[color].maxCol, c);
            }
        }
    }
}
```

**ColorDomain Struct:**
```cpp
struct ColorDomain {
    int minRow = INT_MAX, maxRow = -1;
    int minCol = INT_MAX, maxCol = -1;

    bool contains(int row, int col) {
        return row >= minRow && row <= maxRow &&
               col >= minCol && col <= maxCol;
    }
};
```

---

## 8.4 Contiguity Inference: `inferFromContiguity()` (src/PuzzleSolver.cpp:310)

### Confidence Weight: 2.0

### Logic
If this cell has 2+ neighbors of the same color that are NOT adjacent to each other → this cell likely bridges them.

### Implementation
```cpp
int PuzzleSolver::inferFromContiguity(int row, int col)
{
    int n = puzzle.getOriginal().size();
    std::map<int, std::vector<std::pair<int, int>>> colorRegions;

    // Collect adjacent colored cells
    for (int i = 0; i < 4; i++) {
        int nr = row + directions[i][0];
        int nc = col + directions[i][1];

        if (nr >= 0 && nr < n && nc >= 0 && nc < n) {
            int color = puzzle.getMasked()[nr][nc];
            if (color != -1) {
                colorRegions[color].push_back({nr, nc});
            }
        }
    }

    // If we have 2+ cells of same color that aren't adjacent to each other
    for (auto &[color, cells] : colorRegions) {
        if (cells.size() >= 2) {
            // Check if these cells are disconnected
            bool disconnected = true;
            for (size_t i = 0; i < cells.size() - 1; i++) {
                for (size_t j = i + 1; j < cells.size(); j++) {
                    int dr = abs(cells[i].first - cells[j].first);
                    int dc = abs(cells[i].second - cells[j].second);
                    if (dr + dc == 1) { // Adjacent to each other
                        disconnected = false;
                        break;
                    }
                }
            }

            if (disconnected) {
                return color; // This cell bridges disconnected regions
            }
        }
    }

    return -1;
}
```

**Example:**
```
  2
2 ? 2
  2
```
If the `?` has neighbors of color 2 that aren't touching each other, `?` is likely color 2.

---

## 8.5 Pattern Completion: `inferPatternCompletion()` (src/PuzzleSolver.cpp:193)

### Confidence Weight: 1.5 (Lowest)

### Logic
Detect L-shaped patterns: if cells above and to the left are the same color → infer current cell is same.

### Implementation
```cpp
int PuzzleSolver::inferPatternCompletion(int row, int col)
{
    int n = puzzle.getOriginal().size();

    // Check for L-shape patterns
    if (row > 0 && col > 0) {
        int above = puzzle.getMasked()[row - 1][col];
        int left = puzzle.getMasked()[row][col - 1];
        if (above != -1 && above == left) {
            return above;
        }
    }

    return -1;
}
```

**Pattern:**
```
X
X ?
```
If both X's are same color, infer `?` is same color.

---

## 9. Probing: `probe()` (src/PuzzleSolver.cpp:365)

### Purpose
Reveal the true color of a masked cell by looking at the original matrix.

### Implementation
```cpp
void PuzzleSolver::probe(int row, int col)
{
    probeCount++;
    int colour = puzzle.getOriginal()[row][col];
    std::cout << "Probed color " << colour << " at (" << row << "," << col << ")\n";
    puzzle.getMasked()[row][col] = colour;
}
```

**Key Points:**
- Increments `probeCount` statistic
- Retrieves true color from `original` matrix
- Updates `masked` matrix with revealed color
- **Does NOT modify `currentState`**
- Probes are permanent (not undone during backtracking)

---

## 10. Constraint Validation: `isValid()` (src/PuzzleSolver.cpp:373)

### Purpose
Check if placing a queen at (row, col) violates any constraints.

### Constraints Checked

#### Constraint 1: Column Uniqueness
```cpp
for (int i = 0; i < row; i++) {
    if (puzzle.getCurrentState()[i][col] == 0) {
        return false; // Queen already in this column
    }
}
```

#### Constraint 2: No Adjacent Queens (Diagonal + Orthogonal)
```cpp
for (int i = 0; i < row; i++) {
    for (int j = 0; j < n; j++) {
        if (puzzle.getCurrentState()[i][j] == 0) { // Found a queen
            // Check diagonally adjacent
            if (abs(row - i) == 1 && abs(col - j) == 1) {
                return false;
            }

            // Check horizontally/vertically adjacent
            if ((abs(row - i) == 1 && col == j) || (row == i && abs(col - j) == 1)) {
                return false;
            }
        }
    }
}
```

#### Constraint 3: Color Uniqueness
```cpp
int currentColour = puzzle.getMasked()[row][col];

for (int i = 0; i < row; ++i) {
    for (int j = 0; j < n; ++j) {
        if (puzzle.getCurrentState()[i][j] == 0 && puzzle.getMasked()[i][j] == currentColour) {
            return false; // Another queen in same color
        }
    }
}
```

**Summary:**
- No two queens in same column
- No two queens diagonally adjacent (touching)
- No two queens orthogonally adjacent (touching)
- No two queens in same color region

---

## 11. Backtracking: `undoQueenPlacement()` (src/PuzzleSolver.cpp:766)

### Purpose
Remove a queen placement and restore board state.

### Implementation
```cpp
void PuzzleSolver::undoQueenPlacement(int row, int col)
{
    puzzle.getCurrentState()[row][col] = puzzle.getMasked()[row][col]; // Restore color
    queensPlaced--;
    backtrackCount++;
}
```

**Key Points:**
- Restores `currentState[row][col]` to its color value (removes queen marker `0`)
- Decrements `queensPlaced`
- Increments `backtrackCount`
- **Does NOT undo probes** (masked matrix unchanged)

---

## 12. Solution Verification: `verifyQueenPlacement()` (src/PuzzleSolver.cpp:740)

### Purpose
Validate that the final solution satisfies all constraints.

### Process Flow

#### Step 12.1: Find All Queens
```cpp
std::vector<std::pair<int, int>> queensPositions;

for (int r = 0; r < n; r++) {
    for (int c = 0; c < n; c++) {
        if (puzzle.getCurrentState()[r][c] == 0) {
            queensPositions.push_back({r, c});
        }
    }
}
```

#### Step 12.2: Count Verification
```cpp
if (queensPositions.size() != n) {
    std::cout << "❌ WRONG NUMBER OF QUEENS!" << std::endl;
    return;
}
```

#### Step 12.3: Constraint Verification
For each queen pair, check:

**Color Constraint:**
```cpp
if (puzzle.getMasked()[r2][c2] == queenColor) {
    std::cout << "❌ VIOLATION: Multiple queens in same color";
    hasViolations = true;
}
```

**Column Constraint:**
```cpp
if (c2 == c) {
    std::cout << "❌ VIOLATION: Multiple queens in same column";
    hasViolations = true;
}
```

**Diagonal Adjacency:**
```cpp
if (abs(r2 - r) == 1 && abs(c2 - c) == 1) {
    std::cout << "❌ VIOLATION: Queens diagonally adjacent";
    hasViolations = true;
}
```

**Orthogonal Adjacency:**
```cpp
if ((abs(r2 - r) == 1 && c2 == c) || (r2 == r && abs(c2 - c) == 1)) {
    std::cout << "❌ VIOLATION: Queens horizontally/vertically adjacent";
    hasViolations = true;
}
```

---

## 13. Statistics Output: `printStatistics()` (src/PuzzleSolver.cpp:719)

### Metrics Displayed

```cpp
std::cout << "Queen placements: " << queensPlaced << std::endl;
std::cout << "Backtracks: " << backtrackCount << std::endl;
std::cout << "Probe operations: " << probeCount << std::endl;
std::cout << "Inferred operations: " << inferredCount << std::endl;
std::cout << "Sensing budget used: " << (15 - sensingBudget) << "/15" << std::endl;
std::cout << "Remaining budget: " << sensingBudget << std::endl;

double efficiency = (double)queensPlaced / (queensPlaced + backtrackCount + 1);
double inferenceRatio = (double)inferredCount / (inferredCount + probeCount + 1);
double budgetEfficiency = (double)probeCount / 15.0;

std::cout << "Search efficiency: " << efficiency << std::endl;
std::cout << "Inference vs Probe ratio: " << inferenceRatio << std::endl;
std::cout << "Budget efficiency: " << budgetEfficiency << " (lower is better)" << std::endl;
std::cout << "Total operations: " << (queensPlaced + backtrackCount + probeCount + inferredCount) << std::endl;
```

### Key Metrics

**Search Efficiency:**
- Formula: `queensPlaced / (queensPlaced + backtracks + 1)`
- Higher = fewer backtracks needed

**Inference Ratio:**
- Formula: `inferredCount / (inferredCount + probeCount + 1)`
- Higher = more cells inferred vs probed

**Budget Efficiency:**
- Formula: `probeCount / 15.0`
- Lower = fewer probes used

---

## 14. Final Results (back in main.cpp)

### Aggregated Statistics
```cpp
std::cout << "\n======= FINAL RESULTS =======" << std::endl;
std::cout << "Solved: " << solvedCount << "/" << numPuzzles << " puzzles" << std::endl;
std::cout << "Success rate: " << (100.0 * solvedCount / numPuzzles) << "%" << std::endl;
```

---

# NOTES: Unused/Incomplete Features

## 1. Unused Solving Algorithm: `solvePuzzle()` (src/PuzzleSolver.cpp:17)

### Status
**NOT CURRENTLY USED** - Original solving approach with different strategy.

### Key Differences from Current Implementation

**Full Inference Upfront:**
```cpp
performFullInference(n); // Line 26 - runs before attempting placements
```

**Local Active Sensing:**
```cpp
auto bestLocalProbe = selectBestLocalProbe(row, col, n);
if (bestLocalProbe.first != -1 && bestLocalProbe.second != -1) {
    probe(bestLocalProbe.first, bestLocalProbe.second);
}
```

**Column-wise Iteration:**
- Iterates through columns, not rows
- Different from current row-based backtracking

### When It Was Used
This appears to be the original "aggressive inference" approach that was replaced by `solveWithMinimalProbing()` for more realistic minimal sensing behavior.

---

## 2. Unused Full Inference: `performFullInference()` (src/PuzzleSolver.cpp:74)

### Status
**IMPLEMENTED BUT NOT CALLED** in current flow.

### What It Does
Iteratively applies all inference techniques across entire board until no new inferences can be made.

```cpp
void PuzzleSolver::performFullInference(int n)
{
    bool madeInference;
    int passes = 0;

    do {
        madeInference = false;
        passes++;

        for (int r = 0; r < n; r++) {
            for (int c = 0; c < n; c++) {
                if (puzzle.getMasked()[r][c] == -1) {
                    int inferredColor = inferWithConfidence(r, c);
                    if (inferredColor != -1) {
                        puzzle.getMasked()[r][c] = inferredColor;
                        inferredCount++;
                        madeInference = true;
                    }
                }
            }
        }
    } while (madeInference && passes < 10);
}
```

### Why Not Used
Commented out in `solveWithMinimalProbing()` line 827: reveals too much information upfront, defeating the "minimal probing" research goal.

### Potential Use Cases
- Pre-processing step to reduce search space
- Comparison baseline for evaluating lazy vs aggressive inference
- Debugging/validation tool

---

## 3. Unused Probe Selection Functions

### 3.1 `selectBestProbeTarget()` (src/PuzzleSolver.cpp:488)

**Status:** Implemented but NOT called in current flow.

**Purpose:** Globally evaluate all unknown cells and select the most informative probe.

**Implementation:**
```cpp
std::pair<int, int> PuzzleSolver::selectBestProbeTarget(int n)
{
    double bestValue = -1.0;
    std::pair<int, int> bestTarget = {-1, -1};

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

    if (bestValue > 2.0) {
        return bestTarget;
    }
    return {-1, -1};
}
```

**When It Was Used:** In the old `solvePuzzle()` algorithm for global probe selection.

---

### 3.2 `selectGlobalBestProbe()` (src/PuzzleSolver.cpp:543)

**Status:** Implemented but NOT called.

**Purpose:** Enhanced version of `selectBestProbeTarget()` with lookahead bonuses.

**Key Feature - Cascade Detection:**
```cpp
int unknownNeighborsOfNeighbors = 0;
// Count neighbors of neighbors (cascade effect)
lookaheadBonus = unknownNeighborsOfNeighbors * 0.5;
value += lookaheadBonus;
```

Rewards positions that could trigger inference cascades.

---

### 3.3 `shouldSkipCurrentPosition()` (src/PuzzleSolver.cpp:516)

**Status:** Implemented but NOT called.

**Purpose:** Decide if current position should be skipped in favor of a globally better probe.

**Logic:**
```cpp
bool shouldSkip = globalBestValue > (currentValue * skipThreshold);
// skipThreshold = 1.5
```

If global best is 1.5x better, skip current position.

---

### 3.4 `selectBestLocalProbe()` (src/PuzzleSolver.cpp:678)

**Status:** Implemented but NOT called in current flow.

**Called By:** Old `solvePuzzle()` algorithm only.

**Purpose:** Simulate nearby probe candidates and select best based on inference potential.

**Process:**
1. Get local candidates (2-cell radius)
2. Simulate each probe outcome
3. Score based on inferences enabled
4. Require 20% improvement to switch from current position

---

### 3.5 `getLocalProbeCandidate()` (src/PuzzleSolver.cpp:594)

**Status:** Helper for `selectBestLocalProbe()` - NOT used in current flow.

**Purpose:** Find unknown cells within 2-cell radius, limit to top 4 by distance.

---

### 3.6 `simulateProbeOutcome()` (src/PuzzleSolver.cpp:634)

**Status:** Helper for `selectBestLocalProbe()` - NOT used in current flow.

**Purpose:** Temporarily probe a cell, count how many inferences it enables, then restore state.

**Scoring:**
```cpp
int score = newInferences * 10;
score += 5; // Helps current row
// + corner/edge bonuses
```

---

## 4. Unused Probe Value Calculator: `calculateProbeValue()` (src/PuzzleSolver.cpp:416)

**Status:** Implemented but NOT called in current flow.

**Purpose:** Heuristically score how valuable probing a position would be.

**Factors:**
1. Unknown neighbors count × 2.0
2. Corner: +1.5, Edge: +1.0
3. Multiple neighbor colors: +3.0 (bridge point)
4. Row without queen: +1.5

**Used By:** The unused probe selection functions above.

---

## 5. Sensing Budget (Not Enforced)

### Current State
```cpp
int sensingBudget = 15;  // Declared in PuzzleSolver.h
```

**NOT enforced** in `probe()` function - no check or decrement.

### How to Enforce
Add to `probe()`:
```cpp
void PuzzleSolver::probe(int row, int col)
{
    if (sensingBudget <= 0) {
        std::cout << "❌ Sensing budget exhausted!\n";
        return; // Or throw exception
    }

    sensingBudget--;  // Decrement budget
    probeCount++;
    int colour = puzzle.getOriginal()[row][col];
    puzzle.getMasked()[row][col] = colour;
}
```

### Research Implications
- Currently: unlimited probes allowed
- With budget: forces solver to be more strategic
- Could trigger early termination if budget exhausted before solution found

---

## 6. Missing Function: `isProbeNecessaryForQueenPlacement()`

### Status
**DECLARED in header but NOT IMPLEMENTED**

**Declaration (PuzzleSolver.h:42):**
```cpp
bool isProbeNecessaryForQueenPlacement(int row, int col);
```

### No implementation exists in PuzzleSolver.cpp

### Recommended Implementation

This function could determine if probing is required to make progress:

```cpp
bool PuzzleSolver::isProbeNecessaryForQueenPlacement(int row, int col)
{
    // If cell already known, no probe needed
    if (puzzle.getMasked()[row][col] != -1) {
        return false;
    }

    // Try inference first
    int inferred = inferWithConfidence(row, col);
    if (inferred != -1) {
        return false; // Can infer, no probe needed
    }

    // Check if this is the only viable position in current row
    auto viablePositions = findViableQueenPositions(row, puzzle.getSize());
    int unknownViableCount = 0;

    for (auto [r, c] : viablePositions) {
        if (puzzle.getMasked()[r][c] == -1) {
            unknownViableCount++;
        }
    }

    // If this is the only unknown viable position, probe is necessary
    if (unknownViableCount == 1 && puzzle.getMasked()[row][col] == -1) {
        return true;
    }

    // Check if color information is needed for constraint validation
    // (e.g., to check if another queen already in this color)
    return true; // Conservative: probe when in doubt
}
```

### Use Case
Could replace `needsProbeForDecision()` or be used for more sophisticated probe-avoidance logic.

---

## 7. Unused Smart Masking: `createSmartMaskedMatrix()` (src/graph.cpp:160)

### Status
**IMPLEMENTED BUT NOT USED**

Currently using: `createMaskedMatrix(data, 0.2)` (random 20% masking)

### Smart Masking Features

**1. Preserve Single-Cell Colors:**
```cpp
if (count == 1) {
    preservePositions.insert(colorPositions[color][0]);
}
```

**2. Conservative Masking for Small Regions:**
```cpp
if (colorCounts[color] <= 3) {
    std::bernoulli_distribution conservativeMask(mask_prob * 0.5);
}
```

**3. Edge/Corner Preference:**
```cpp
if (i == 0 || i == n-1 || j == 0 || j == n-1) {
    std::bernoulli_distribution edgeMask(mask_prob * 0.7);
}
```

**4. Ensure Each Color Has One Visible Cell:**
```cpp
if (!hasVisible && !positions.empty()) {
    int revealIdx = gen() % positions.size();
    masked[revealPos.first][revealPos.second] = color;
}
```

### Why Use Smart Masking?
- More realistic puzzles (preserves solvability)
- Better for testing inference algorithms
- Ensures each color region is discoverable

### How to Enable
Change Graph constructor (graph.cpp:48):
```cpp
// From:
: original(data), currentState(data), masked(createMaskedMatrix(data, 0.2))

// To:
: original(data), currentState(data), masked(createSmartMaskedMatrix(data, 0.2))
```

---

## 8. Unused Graph Print Modes

### Currently Used
- `MASKED`: Show initial partially-revealed board
- `ORIGINAL`: Show complete solution board
- `CURRENT_SYMBOLS`: Show queen placements as Q/.

### Not Used
- `CURRENT_RAW`: Show current state with color numbers (queens as 0)

**Could be useful for debugging** to see queen markers alongside color values.

---

## 9. Unused/Commented Code Blocks

### In main.cpp:11-12
```cpp
// std::vector<Graph> Graphs; = PuzzleManager::loadFromFile(puzzleFileName, numPuzzles);
```
Old return-style API (now uses reference parameter).

### In main.cpp:48-50
```cpp
// if (!g.solvePuzzle(0, g.getSize())) {
//     std::cout << "No solution found.\n";
// }
```
Old direct call to Graph's solve method (moved to PuzzleSolver).

### In PuzzleSolver.cpp:11-14
```cpp
// int queensPlaced = 0;
// int backtrackCount = 0;
// int probeCount = 0;
// int inferredCount = 0;
```
These are member variables, not needing initialization in constructor (have default values).

### In PuzzleSolver.cpp:803-804
```cpp
// performFullInference(n); // This was revealing too much - comment out
```
Intentionally disabled for minimal probing approach.

---

# RECOMMENDATIONS

## 1. Enable Sensing Budget Enforcement
Add budget checking in `probe()` to truly limit sensing operations. This would make the research more realistic.

## 2. Implement `isProbeNecessaryForQueenPlacement()`
Could provide more sophisticated probe-avoidance logic, potentially reducing probe count.

## 3. Use Smart Masking for Better Puzzles
`createSmartMaskedMatrix()` would create more realistic test cases and ensure all puzzles are solvable.

## 4. Add Comparative Metrics
Track performance differences between:
- `solveWithMinimalProbing()` vs `solvePuzzle()`
- Random masking vs smart masking
- With/without budget enforcement

## 5. Inference Technique Effectiveness
Add counters for each inference technique:
```cpp
int neighborInferCount = 0;
int uniformityInferCount = 0;
int domainInferCount = 0;
int contiguityInferCount = 0;
int patternInferCount = 0;
```

Track which techniques are most effective for different puzzle types.

## 6. Global vs Local Probe Selection Comparison
Create experiments comparing:
- Current on-demand probing
- Global best probe selection
- Local simulation-based selection

Measure probe efficiency differences.

## 7. Adaptive Threshold Tuning
The `maxConfidence = 3.0` threshold in `inferWithConfidence()` is hardcoded. Could be tuned based on:
- Board size
- Masking percentage
- Current solve progress

## 8. Probe Cascade Tracking
Implement metrics to track how many inferences each probe enables:
```cpp
int cascadeDepth = performCascadeInference(row, col);
probeCascadeMetrics.push_back(cascadeDepth);
```

Would quantify probe value in practice.

## 9. Failure Analysis
When puzzles fail, log:
- How many cells still masked
- Which row failed
- Available sensing budget
- Last successful inference technique

## 10. Interactive Mode
Add a mode where the solver asks before probing:
```cpp
if (interactiveMode) {
    std::cout << "Probe (" << row << "," << col << ")? (y/n): ";
    char response;
    std::cin >> response;
    if (response != 'y') return;
}
```

Useful for understanding solver behavior and debugging.

---

# AREAS NOT FUNCTIONING PROPERLY

## 1. Sensing Budget Display (src/PuzzleSolver.cpp:627)
```cpp
std::cout << "Sensing budget used: " << (15 - sensingBudget) << "/15" << std::endl;
std::cout << "Remaining budget: " << sensingBudget << std::endl;
```

**Issue:** `sensingBudget` never changes from initial value (15), so always shows:
- Sensing budget used: 0/15
- Remaining budget: 15

**Should show:** Actual `probeCount` instead.

**Fix:**
```cpp
std::cout << "Probes used: " << probeCount << std::endl;
```

## 2. Budget Efficiency Metric (src/PuzzleSolver.cpp:633)
```cpp
double budgetEfficiency = (double)probeCount / 15.0;
```

**Issue:** Divides by hardcoded 15.0, but if board size varies, the "budget" concept doesn't scale.

**Recommendation:** Use board size or configurable budget:
```cpp
double budgetEfficiency = (double)probeCount / (double)n; // probes per row
```

## 3. Missing Error Handling in PuzzleManager::loadFromFile()

**Issue:** If file open fails, function continues anyway:
```cpp
if (!puzzleFile.is_open()) {
    std::cerr << "Unable to open file: " << filename << std::endl;
    // return graphs;  // COMMENTED OUT - doesn't actually return!
}
```

**Result:** Proceeds to read from invalid file stream, likely crashes or produces garbage data.

**Fix:**
```cpp
if (!puzzleFile.is_open()) {
    std::cerr << "Unable to open file: " << filename << std::endl;
    return; // Actually return on error
}
```

## 4. Unused Header Function Declaration

`isProbeNecessaryForQueenPlacement()` is declared in header but never implemented - will cause linker error if anything tries to call it.

**Fix:** Either implement it or remove from header.

## 5. Potential Infinite Loop in performFullInference()

Protected by `passes < 10` limit, but if inference techniques have bugs, could waste computation in early passes.

**Recommendation:** Add early exit if no progress:
```cpp
if (passes > 3 && !madeInference) {
    break; // No point continuing
}
```

---

# SUMMARY

## Current Active Flow
1. **Load puzzles** from file → Create Graphs with 20% random masking
2. **For each puzzle:**
   - Create PuzzleSolver
   - Call `solveWithMinimalProbing(n)`
     - Recursively solve row-by-row with `solveRowWithBacktracking()`
     - For each viable position:
       - Try inference first via `needsProbeForDecision()`
         - Uses 5 inference techniques with confidence scoring
       - Probe only if inference fails
       - Validate with `isValid()` (3 constraints)
       - Place queen or backtrack
3. **Verify solution** and print statistics

## Key Design: Lazy Inference
- No upfront full board inference
- Inference attempted only when needed for decision
- Probes are permanent (not undone)
- Backtracking only undoes queen placements

## Major Unused Components
- `solvePuzzle()` - old algorithm
- `performFullInference()` - aggressive inference
- All global/local probe selection functions
- Smart masking strategy
- Sensing budget enforcement
