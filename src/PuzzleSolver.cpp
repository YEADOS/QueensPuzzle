#include "../include/PuzzleSolver.h"
#include "../include/graph.h"
#include <set>
#include <map>

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
        // IMPROVEMENT 2: Only probe if still unknown after all inference attempts
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
                // Only probe as last resort
                probe(row, col);
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
    int currentColour = puzzle.getCurrentState()[row][col];

    // Check to see if the queen is in the same column
    for (int i = 0; i < row; i++)
    {
        if (puzzle.getCurrentState()[i][col] == 0)
        {
            return false;
        }
    }

    // Check diagonally adjacent (lower-left)
    if (row > 0 && col > 0 && puzzle.getCurrentState()[row - 1][col - 1] == 0)
        return false;

    // Check diagonally adjacent (upper-right)
    if (row > 0 && col < n - 1 && puzzle.getCurrentState()[row - 1][col + 1] == 0)
        return false;

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

void PuzzleSolver::printStatistics()
{
    std::cout << "\n=== Solver Statistics ===" << std::endl;
    std::cout << "Queen placements: " << queensPlaced << std::endl;
    std::cout << "Backtracks: " << backtrackCount << std::endl;
    std::cout << "Probe operations: " << probeCount << std::endl;
    std::cout << "Inferred operations: " << inferredCount << std::endl;

    // Calculate efficiency
    double efficiency = (double)queensPlaced / (queensPlaced + backtrackCount);
    std::cout << "Efficiency ratio: " << efficiency << std::endl;
}
