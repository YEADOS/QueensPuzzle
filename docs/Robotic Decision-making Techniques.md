# Robotic Decision-Making Techniques (LinkedIn Queens Puzzle)

## Purpose  
In environments where rules are not fully known or change over time, robots must make decisions dynamically. This mirrors how the LinkedIn Queens Puzzle must be solved when constraint regions (colours) are hidden. The aim is to simulate robotic adaptability through hybrid logic + behaviour models.

---

## Core Concepts

### 1. **Exploration-Based Reasoning**
- The system “tests” the board by placing queens to observe outcomes.
- Inspired by:
  - Frontier-based exploration
  - Bayesian optimization
  - Information gain strategies
-  *In the puzzle: placing a queen in an unknown region reveals feedback about constraints.*

---

### 2. **Reinforcement Learning (RL)**
- Learns correct placements via trial and error.
- Sparse feedback is typical: most placements fail, a few succeed.
- Can use **curriculum learning**: start with small boards, scale up.

```cpp
// Example reward system:
+1 = valid placement
-1 = conflict
0 = uncertain region
```

### 3. POMDP (Partially Observable Markov Decision Process)
- The agent maintains a **belief** about what regions are valid.
- Each action (queen placement) updates its **belief state**.
- Models the **decision-making process under partial knowledge**.
- ⚠️ High computational cost, but useful for simulating reasoning under uncertainty.

---

### 4. SLAM-like Inference
- Unknown colour regions are treated like **unexplored terrain**.
- Analogous to **SLAM** (Simultaneous Localization and Mapping) in robotics.
- Each queen placement = mapping part of the board.
- Can be paired with constraint solvers for **layered reasoning**.

---

### 5. Constraint Learning
- The agent **learns constraints through repeated interaction**.
- Example: “Queens can’t go in red zones” is not predefined — it's **inferred from failures**.
- Useful when **generalising across unfamiliar puzzle boards**.

---

### Hybrid Execution Strategy
A smart robot would combine **logical reasoning** and **adaptive behaviour**.  
This system simulates that approach:

1. **Explore** → place queens in unknown zones to test constraints  
2. **Infer** → update internal state based on outcomes  
3. **Solve** → apply CSP algorithms (e.g. backtracking with MRV/LCV)  
4. **Repeat** → alternate between exploration and solving until complete

This approach mimics how real robots refine decisions over time.

---

##  Summary Table

| Technique             | Use Case                        | Strengths                          |
|-----------------------|----------------------------------|-------------------------------------|
| Active Sensing        | Probe unknown regions            | Fast feedback, minimal waste        |
| Reinforcement Learning| Learn optimal moves via feedback| Adapts over time                    |
| POMDP                 | Decision-making under uncertainty| Models belief-based reasoning       |
| SLAM                  | Infer coloured zones like maps   | Terrain mapping analogies           |
| Constraint Learning   | Learn rules through failure      | Improves generalisation             |

---

## Application in Puzzle

These robotic decision-making techniques are integrated into a **hybrid solver** for the *LinkedIn Queens Puzzle*:

- Handles **partially observable puzzles** (unknown colours)
- Balances **logical constraint satisfaction** and **exploration**
- Assists users with **placement suggestions and feedback**
- **Adapts over time** like a learning robot system

---
## References

- Siciliano & Khatib (2016) – *Robotics Handbook*  
- Thrun et al. (1999) – *Probabilistic Robotics*  
- Reiter (2001), Finzi & Pirri (2001) – *Temporal Logic in AI*  
- Arkin (1998) – *Behavior-Based Robotics*