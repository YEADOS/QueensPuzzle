Subject: [[000 Main Notes/Research Project|Research Project]]
Topics: 

___ 
# Introduction

Example board: 
![[Pasted image 20250330203521.png|400]]

## Problem 
* Similar to original problem, however there are a different constraints and conditions. 

**Rules** 
1. Each Row must have exactly one queen (1 per)
	* No Two queens can share a row 
2. Each Column must have exactly one queen (1 per)
	* No Two queens can share a column
3. The coloured section must have only one queen
4. The queens cannot be adjacent to each other diagonally
	* ****note***: queens can be diagonal of each other but not right next to each other (1-unit away)

The base conditions stay the same except that queens can be diagonal of each other if they are not adjacent to each other. 

# Implementation - Solving the problem

 Potential Algorithms:
1. Backtracking 
2. Heuristic Search 
	 * Simulated Annealing 
	 * Greedy
3. AC-3 + Backtracking (Smart Backtracking)  / other Constraint propagation methods 
4. Dancing Links (DLX) (Specialized backtracking)


Integer Programming / SAT Solvers (Tool rather than Algorithm): 
1. [[Z3 Theorem Prover]]


## Backtracking: 

* Backtracking is used to solve various computational problems, notably constraint satisfaction problems.

Python Code Example 
https://towardsdatascience.com/solving-linkedin-queens-game-cfeea7a26e86


## How to solve - Human Approach 

https://www.youtube.com/watch?v=Devp-XMY1uI

* look for columns or rows with only one colour
	* The places outside are invalid for queen
* Look for spots that stop the placement of whole another colour
	* Don't place queens where it prevents a queen to be placed in another colour.
![[Pasted image 20250408192711.png|200]]
* The number of rows or columns that a colour takes up can be used to determine where the queen area is limited to and where it's not worth placing a queen for specific colours
	* e.g. in the problem below the queen for the red section can only be placed in the first two rows, and the yellow queen in the first. This means that there is no point of placing a queen in the green section within these rows. \
* If a colours row or column range is within another colour (sub-set), the super colour with a wider spreading range is not to be chosen within this domain. the super colour should be chosen outside the range of the sub colour area.
	* e.g.
		* Green Row domain (GQ > 0 and GQ <= 5).  
		* Red Row domain (RQ > 0 and RQ <=2 ).
		* Yellow Row domain (YQ >0 and YQ <= 1)
			* this means: 
				* Red is placed from (RQ > 1 and RQ <=2 ) (which is RQ = 2)
				* Yellow should be placed in original domain (YQ = 1)
				* Green should be placed from (GQ > 2 and GQ <=5) 
				* 
![[Pasted image 20250408192557.png|400]]
* Queen can be placed once a coloured section has been limited to only one line (row or column)

### Basic Steps to Solve the Queens Puzzle (from the video)

Here are 4 basic steps to approach solving the Queens puzzle, based on the logic shown in the video:

1.  **Identify Constraints:** Look for the most restricted rows, columns, or regions where a queen *must* be placed due to the limited number of available valid squares [00:02:44]. Start there.
2.  **Place Queen Logically:** Place a queen in one of the highly constrained areas you identified, or deduce a placement based on logic (e.g., if a region only has valid spots in one specific row, the queen for that region must be in that row) [00:04:24].
3.  **Eliminate Attacked Squares:** Immediately mark off all squares in the same row, column, and diagonals from the queen you just placed. Also, remember no other queen can be in the same region [00:02:57].
4.  **Repeat and Iterate:** Find the *next* most constrained area based on the remaining available squares and repeat the process of placing queens and eliminating possibilities until one queen is placed in every row, column, and region [00:06:19].

## Queens Levels

Here is a link to all LinkedIn Queens Levels:  
https://github.com/samimsu/queens-game-linkedin/tree/main/src/utils/levels
* Need a way to convert from typescript into c++ matrix 

The repository for the queens game tool
https://github.com/samimsu/queens-game-linkedin/tree/main/src

# References 

Backtracking - python example
https://towardsdatascience.com/solving-linkedin-queens-game-cfeea7a26e86
https://github.com/RebeccaTadesse/LinkedIn-Queens-Solver?utm_source=chatgpt.com
https://medium.com/%40alene9471/solving-the-n-queen-problem-using-constraint-satisfaction-96b551f80fb6

Brute Force 
https://sheeptester.github.io/words-go-here/misc/queens.html?utm_source=chatgpt.com

Z3 Implementation
thttps://medium.com/towards-data-engineering/solving-linkedins-queens-puzzle-with-code-6f1c3aa23a40

https://github.com/samimsu/queens-game-linkedin/tree/main/src