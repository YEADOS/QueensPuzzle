# Active Sensing and Heuristics in RoboticSolver

## Can the Solver Solve the Board Without Actively Sensing Every Part?

Yes, your solver can solve the board **without actively sensing every part**—this is the essence of combining **active sensing** with **heuristics**. The goal is to only sense (reveal) the minimum necessary parts to satisfy constraints and find a solution.

### How to Achieve This

1. **Track Uncertainty:**  
   - Use `colourMask` to mark which board sections are unknown.
   - Only probe (reveal) a section when the solver cannot proceed without knowing its colour.

2. **Heuristic-Driven Sensing:**  
   - Before probing, use heuristics to decide if sensing is necessary.
   - For example, only sense a section if all possible queen placements in the current column/row are blocked by uncertainty.

3. **Lazy Sensing in Solver:**  
   - In your `solveRecursive` (or equivalent), try to place a queen using only known information.
   - If no valid placement is found, then probe the most promising unknown section (using a heuristic, e.g., most constrained variable).
   - Repeat until a solution is found or all options are exhausted.

#### Example Logic (Pseudo-code)

```cpp
bool RoboticSolver::solveRecursive(int col) {
    if (col == boardGraph.size()) return true;
    for (int row = 0; row < boardGraph.size(); ++row) {
        if (isSafe(row, col)) {
            boardGraph.placeQueen(row, col);
            if (solveRecursive(col + 1)) return true;
            boardGraph.removeQueen(row, col);
        }
    }
    // If no placement is possible, probe the most promising unknown section
    int probeRow, probeCol;
    if (findMostPromisingUnknown(probeRow, probeCol)) {
        probe(probeRow, probeCol);
        return solveRecursive(col); // Retry with new information
    }
    return false;
}
```

### Summary

- **Do not sense every part up front.**
- **Sense only when necessary, guided by heuristics.**
- **Try to solve with current knowledge first, probe only when blocked.**

This approach mimics how a robot or human would solve a puzzle with partial information, minimizing unnecessary exploration.

---

## `src/graph.cpp` (Reference Implementation)

Your `src/graph.cpp` file provides the board representation and basic queen placement logic. The `isValid` and `solvePuzzle` functions are used to check constraints and perform backtracking. The robotic solver should use these functions, adding active sensing and heuristics as described above.
