# Snake Game (Raylib) - Project Report

## Title
Snake Game with A* Path Visualization

## Abstract
This project implements a classic Snake game in C++ using raylib, with a grid-based playfield, static walls, and an A* pathfinding visualization to the food when paused. The goal was to build a clean, modular game loop with simple AI tooling that could be inspected during runtime. The implementation emphasizes clarity, determinism in movement, and collision safety.

---

## 1. Core Content

### 1.1 Overview
The game runs in a fixed-size window (`800x600`) with a tile size of `25px`, yielding a `32x24` grid. The snake moves in tile increments at a fixed delay, food spawns on unoccupied tiles, and collisions end the game. When the game is paused, an A* search displays the shortest path from the snake's head to the food, visualized tile-by-tile.

### 1.2 Key Functionality
- **Grid rendering:** Alternating colors for a readable playfield and static blue wall clusters.
- **Snake movement:** Tile-based movement with growth on food collision.
- **Food spawning:** Randomized placement that avoids snake body and walls.
- **Pathfinding (A*):** Shortest path computed on pause, shown progressively for clarity.

### 1.3 Code Snippets
Use these snippets in the final report, with line numbers/labels as needed.

**Main loop (movement + pause + A*):**
```cpp
if (IsKeyPressed(KEY_P)) {
    paused = !paused;
    if (paused) {
        path = AStar::FindPath(g, s, f.getPos(), windowWidth, windowHeight);
        visiblePathTiles = 0;
        lastPathStepTime = GetTime();
    }
}

if (!paused && GetTime() - lastMoveTime >= moveDelay) {
    s.move();
    lastMoveTime = GetTime();
}
```

**A* core idea (grid, Manhattan heuristic):**
```cpp
static float Heuristic(int x, int y, int gx, int gy) {
    return static_cast<float>(abs(gx - x) + abs(gy - y));
}
```

### 1.4 Visuals (Insert Your Own)
Add your own screenshots or short clips. Label them and reference in the text.

- **Figure 1:** Normal gameplay (snake, food, walls).
- **Figure 2:** Paused view with A* path tiles in yellow.
- **Figure 3:** Food spawn example showing obstacle avoidance.

Example Markdown:
```md
![Figure 1: Normal gameplay](assets/fig1-gameplay.png)
```

### 1.5 Design Choices (Why These Approaches)
- **Raylib:** Minimal setup, fast iteration, and simple 2D primitives.
- **Tile-based grid:** Simplifies movement, collision, and pathfinding.
- **A* with Manhattan heuristic:** Optimal and efficient for 4-direction grid movement.
- **Static walls:** Adds spatial variety and forces pathfinding to be meaningful.

### 1.6 Limitations and Future Work
- **Game end handling:** Currently ends by closing the window; a proper game-over state could improve UX.
- **Wall data:** Hard-coded wall clusters; could be data-driven or randomized.
- **No score UI:** Growth is visible, but score is not displayed.
- **No self-collision grace:** Immediate termination can feel abrupt.
- **Path visualization:** Only on pause; real-time AI/autoplay could be explored.

---

## 2. Project Management

### 2.1 Planning
Briefly describe your plan (example below, edit to match what you did):
- **Phase 1:** Core gameplay loop (window, grid, snake movement).
- **Phase 2:** Food spawning and collision handling.
- **Phase 3:** Static obstacles (walls).
- **Phase 4:** A* pathfinding + pause visualization.
- **Phase 5:** Polish and bug fixes.

### 2.2 Progress Tracking
Describe how you tracked progress (Git commits, checklist, Trello, notes, etc.).

Example:
- Kept a small checklist in a text file and used Git commits to checkpoint features.
- Validated each phase with a quick manual playtest.

### 2.3 Timeline (Example)
- **Week 1:** Core gameplay loop + grid rendering.
- **Week 2:** Food logic + collision checks.
- **Week 3:** Walls + A* pathfinding.
- **Week 4:** Cleanup, visuals, report.

---

## 3. Reflective Element

### 3.1 Biggest Problems Encountered
- **Obstacle-aware spawning:** Food spawning inside walls/snake initially occurred. Fixed by checking tile occupancy before confirming a position.
- **Pathfinding correctness:** Ensured the snake's head tile is allowed and body tiles are blocked.
- **Coordinate conversions:** Mixed pixel vs tile coordinates; resolved by using tile size consistently for conversions.

### 3.2 Learning Outcomes
- **Grid reasoning:** Reinforced how grid-based movement simplifies collision and AI.
- **Search algorithms:** Implemented A* in a real-time context and tuned it for clarity.
- **Code structure:** Separating `Snake`, `Food`, `Grid`, and `AStar` improved maintainability.

### 3.3 What I Would Do Differently
- Add a full game-state machine (menu, playing, paused, game over).
- Add dynamic difficulty (speed increases, randomized wall layouts).
- Add score UI, sound effects, and input buffering for smoother control.

### 3.4 Motivation (Why This Project)
This project connected to an earlier first-year tic-tac-toe assignment where I used a 2D array and ASCII output to navigate a grid. I wanted to revisit that idea with stronger C++ skills and proper visuals. Although graphics were not required, I had been experimenting with raylib and found it simple and productive. A Snake game felt like the best way to showcase A* pathfinding on a grid while still aligning with my interest in game development.

---

## 4. References
List external sources you used here. If none, say so.

Example:
- raylib documentation.
- A* pathfinding references (if used).

---

## Appendix (Optional)
Add build/run instructions or extra screenshots here.

Example:
- Build in Visual Studio using `SnakeGame.sln`.
- Run the compiled executable.
