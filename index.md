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
- **Snake movement:** Discrete, tile-based movement with growth on food collision.
- **Food spawning:** Randomized placement that avoids snake body and walls.
- **Pathfinding (A*):** Shortest path computed on pause, shown progressively for clarity.

### 1.3 Code Walkthrough (By File)

#### Grid (`grid.h` / `grid.cpp`)
I started by building the grid system. The grid is a 2D vector of integers (initialized to 0) used to represent tile positions.

```cpp
class Grid {
private:
    int tileSize_;
    std::vector<std::vector<int>> vec_;
    Color pColour_;
    Color aColour_;
    std::vector<std::vector<Vector2>> walls_;
    void initWalls();
public:
    Grid(int tileSize,
        const std::vector<std::vector<int>>& grid,
        Color primary,
        Color accent);
    void drawGrid();
    void drawWalls();
};
```

The grid is created in `main.cpp` like this:

```cpp
Grid g(
    tileSize,
    std::vector<std::vector<int>>(windowHeight / tileSize, std::vector<int>(windowWidth / tileSize, 0)),
    { 0, 102, 51, 255 },
    { 0, 153, 76, 255 }
);
```

It is drawn using `drawGrid()`, which alternates colors for a checkerboard effect:

```cpp
void Grid::drawGrid()
{
    for (int j = 0; j < (int)vec_.size(); j++)
    {
        for (int i = 0; i < (int)vec_[j].size(); i++)
        {
            if ((i + j) % 2 == 0)
                DrawRectangle(i * tileSize_, j * tileSize_, tileSize_, tileSize_, pColour_);
            else
                DrawRectangle(i * tileSize_, j * tileSize_, tileSize_, tileSize_, aColour_);
        }
    }
}
```

This function walks the grid by row (`j`) and column (`i`).

- If `i + j` is even, it uses the primary color.
- If `i + j` is odd, it uses the accent color.

Example parity pattern:
```
(j,i) grid with (i+j) parity
j=0: (0,0)E (0,1)O (0,2)E (0,3)O
j=1: (1,0)O (1,1)E (1,2)O (1,3)E
j=2: (2,0)E (2,1)O (2,2)E (2,3)O

E = even ? primary color
O = odd  ? accent color
```

I added getters/setters as needed to reduce clutter while building features.

#### Snake (`snake.h` / `snake.cpp`)
I store the snake as a `std::vector<Vector2>` so each body segment has an `(x, y)` position.

```cpp
class Snake {
private:
    std::vector<Vector2> body_;
    Color colour_;
    int dir_ = 0;
public:
    Snake(Color, int);
    void drawSnake();
    void addPart();
    void move();
    void setDir(int);
    int getDir();
    int getSize();
    std::vector<Vector2> getBody() const;
    void boundaryCheck(Grid g);
};
```

- **`drawSnake()`** renders each segment as a rectangle.
- **`addPart()`** grows the snake by duplicating the last segment.
- **`move()`** inserts a new head based on direction and removes the tail.
- **`boundaryCheck()`** ends the game if the snake hits the wall, itself, or the static obstacles.

Movement uses tile-sized steps so the snake stays aligned to the grid.

#### Food (`food.h` / `food.cpp`)
The food system handles spawning and respawning after collision with the snake.

```cpp
class Food {
private:
    int tileSize_ = 25;
    Color colour_;
    Vector2 pos_;
public:
    Food(int, Color, int, int, const Grid&, const Snake&);
    void drawFood();
    void detectCollisionAndMove(Snake&, const Grid&, int a, int b);
    Vector2 getRandom(const Grid&, const Snake&, int, int);
    Vector2 getPos() const { return pos_; }
};
```

Key idea:
- **`getRandom()`** picks a random tile that is not occupied by the snake body or walls.
- **`detectCollisionAndMove()`** checks if the head hits the food, grows the snake, and respawns the food.

This avoids frustrating cases where food spawns inside obstacles.

#### A* Pathfinding (`aStar.h` / `aStar.cpp`)
I implemented A* to compute a shortest path from the snake head to the food.

```cpp
static float Heuristic(int x, int y, int gx, int gy) {
    return static_cast<float>(abs(gx - x) + abs(gy - y));
}
```

Design points:
- Uses Manhattan distance, which matches 4-direction grid movement.
- Treats walls and the snake body (except the head) as blocked.
- Returns a vector of tile coordinates so the path can be drawn as grid-aligned rectangles.

The path is computed only when paused (`P`) and revealed tile-by-tile for clarity.

#### Main Loop (`main.cpp`)
The game loop ties everything together:
- Creates the window and grid
- Handles input and direction changes
- Updates movement on a fixed delay
- Checks food collisions
- Handles pause + A* visualization

```cpp
if (!paused && GetTime() - lastMoveTime >= moveDelay) {
    s.move();
    lastMoveTime = GetTime();
}

if (!paused) f.detectCollisionAndMove(s, g, windowWidth, windowHeight);
```

When paused, the A* path is computed once and then gradually drawn in yellow tiles to show the path clearly.

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
- **Phase 1:** Core gameplay loop (window, grid, snake movement).
- **Phase 2:** Food spawning and collision handling.
- **Phase 3:** Static obstacles (walls).
- **Phase 4:** A* pathfinding + pause visualization.
- **Phase 5:** Polish and bug fixes.

### 2.2 Progress Tracking
- Kept a small checklist and used Git commits to checkpoint features.
- Validated each phase with manual playtests.

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
