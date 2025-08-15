Date: 2025-03-30 18:28
Tags/Topics:
___

# Initial Ideas

* First Document![[Research Project 3.docx]]
* Research Report ![[Preliminary Research Report 1.docx]]
# Topic / Overview

***Initial Topic***: Queens Puzzle Algorithms inspired by Robot Decision Making

* [N-Queens Puzzle](#N-Queens%20Puzzle)
* [LinkedIn Queens Puzzle](#LinkedIn%20Queens%20Puzzle)
* [Variants](#Variants)
* [References](#References)

# N-Queens Puzzle 

## Problem
* Base problem is placing n number of queens on a n x n chessboard where no two queens can attack or "threaten" each other

This means that it has to follow these conditions:
1. No queens share the same row
2. No queens share the same column 
3. No queens are diagonal each other.

## Solutions

* This base problem of the puzzle has solutions for all n values up to n=27, except when n = 2 or n = 3. The puzzle cannot be solved for these two. [Wiki](https://en.wikipedia.org/wiki/Eight_queens_puzzle)

## Algorithms

There are a few different algorithmic approaches to the puzzle. Two of the most popular are: 
1. Backtracking
2. Min-conflicts

Newer Proposed Solution: 
* [[NSCR Algorithm]]
* [(Moghimi & Amini, 2024)]([https://doi.org/10.3390/electronics13204065](https://doi.org/10.3390/electronics13204065))

<h3 style="text-align:center;">Backtracking in Queens Puzzle</h3>
#### Steps - Basic Approach

1. Start with an empty board (N x N board)
2. Place a queen in the first column (or row), procced to next
3. For each column (or row), we place a queen in a row (or column), checking for conflicts
	* Same row, diagonal
4. If safe move to next column
5. If not safe, backtrack to the previous column and try a different row
6. Once all queens are placed without issues, solution is found

#### Complexity:

This the time complexity for optimized backtracking

| Type  | Complexity |     |
| ----- | ---------- | --- |
| Time  | *O(n!)*    |     |
| Space | *O(n)*     |     |
1. [Complexity Reference 1](https://medium.com/better-programming/backtracking-n-queen-problem-and-sudoku-31974988bcb0)
2. 

- **Time Complexity:** `O(n!)`  
  Each queen must be placed in a different row and column. In the worst case, we try all `n` columns for the first queen, `n-1` for the second, and so on — leading to `n!` possible arrangements. Backtracking helps prune invalid branches, but the worst-case is factorial.

- **Space Complexity:** `O(n)`  
  We use recursion up to `n` levels deep and arrays to track used columns and diagonals — each requiring linear space.

#### Optimized Improvements 

- **Benefit:** Makes the algorithm faster by avoiding moves that are clearly not allowed.
- **How:**  
  - It keeps track of which columns and diagonals are already attacked by other queens.
  - It skips over any position that would lead to a conflict, so it doesn’t waste time checking the rest of that path.

This makes the solution much faster in practice, improving the average-case time complexity even though the worst-case time is still `O(n!)`.


#### Code Example 

Base
```python 

function solveNQueens(n):
    result = []
    board = array of size n

    backtrack(row = 0)

    return result

function backtrack(row):
    if row == n:
        add board configuration to result
        return

    for col from 0 to n - 1:
        if is_safe(row, col, board):
            board[row] = col
            backtrack(row + 1)

function is_safe(row, col, board):
    for r from 0 to row - 1:
        if board[r] == col or abs(board[r] - col) == abs(r - row):
            return false
    return true
```

Optimized
```python
function solveNQueens(n):
    result = []
    board = array of size n
    columns = empty set
    diag1 = empty set  // row - col
    diag2 = empty set  // row + col

    backtrack(row = 0)

    return result

function backtrack(row):
    if row == n:
        add board configuration to result
        return

    for col from 0 to n - 1:
        if col in columns or (row - col) in diag1 or (row + col) in diag2:
            continue

        board[row] = col
        columns.add(col)
        diag1.add(row - col)
        diag2.add(row + col)

        backtrack(row + 1)

        columns.remove(col)
        diag1.remove(row - col)
        diag2.remove(row + col)

```

#### Visual

![[Eight-queens-animation.gif]]

<h3 style="text-align:center;">Min-Conflicts</h3>

![[8queensminconflict.gif]]

## Reflection

* The puzzle changes when the conditions change or new constraints are added
* The algorithm to decide and choose where to place the queen reflects robots decision making - They need predefined logic and plans.
* The base problem has mainly be solved already. 


# LinkedIn Queens Puzzle

Game: https://www.linkedin.com/games/queens/

[[LinkedIn Queens Puzzle]]

# Variants 

List of other constraint satisfaction problems and combinatorial search problems:
1. Knights tour
2. Sudoku Solver 
3. Sudoku variants
4. Using other pieces (Knights, Bishops, Rooks )
5. Mixing queens and other pieces
6. 

# Papers 

[[Solving Scheduling problems as the puzzle games using constraint programming]]


# References

https://en.wikipedia.org/wiki/Eight_queens_puzzle

https://medium.com/better-programming/backtracking-n-queen-problem-and-sudoku-31974988bcb0

https://towardsdatascience.com/solving-linkedin-queens-game-cfeea7a26e86/