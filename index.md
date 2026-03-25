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

**`drawWalls()`** renders fixed wall clusters (stored as tile coordinates) and converts them into pixel rectangles at draw time. This keeps wall placement grid-aligned and simple to reason about.

```cpp
void Grid::drawWalls()
{
    for (const auto& wall : walls_)
    {
        for (const auto& vec : wall)
        {
            DrawRectangle(vec.x * tileSize_, vec.y * tileSize_, tileSize_, tileSize_, BLUE);
        }
    }
}
```

Walls are defined once in `initWalls()` as vectors of tile coordinates. Using tile space here avoids off‑by‑one issues and makes it easy to experiment with layouts.

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

**`boundaryCheck()`** ends the game if the head collides with the screen edges, its own body, or any wall tile. This centralizes collision termination logic in one place.

```cpp
void Snake::boundaryCheck(Grid g)
{
    std::vector<std::vector<Vector2>> walls_ = g.getWalls();
    int tileSize_ = g.getTileSize();
    Vector2 head = body_.front();

    if (head.x < 0 || head.x > 800 || head.y < 0 || head.y > 600) {
        CloseWindow();
    }

    for (int i = 1; i < static_cast<int>(body_.size()); ++i)
    {
        if (head.x == body_[i].x && head.y == body_[i].y) {
            CloseWindow();
        }
    }

    for (const auto& wall : walls_)
    {
        for (const auto& vec : wall)
        {
            if (head.x == vec.x * tileSize_ && head.y == vec.y * tileSize_) {
                CloseWindow();
            }
        }
    }
}
```

This keeps collision checks explicit and readable. A natural future improvement would be returning a game state instead of immediately closing the window.

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

**`detectCollisionAndMove()`** checks for head–food overlap and handles growth + respawn. The method is short but important because it controls progression and difficulty.

```cpp
void Food::detectCollisionAndMove(Snake& s, const Grid& g, int a, int b)
{
    std::vector<Vector2> sb = s.getBody();

    if (pos_.x == sb.front().x && pos_.y == sb.front().y) {
        Vector2 random = getRandom(g, s, a, b);
        pos_.x = random.x;
        pos_.y = random.y;

        s.addPart();
        }
}
```

**`getRandom()`** retries random tiles until it finds a free spot. It converts all candidate tiles into grid coordinates to avoid pixel‑precision mistakes.

```cpp
Vector2 Food::getRandom(const Grid& g, const Snake& s, int a, int b)
{
    const int cols = a / tileSize_;
    const int rows = b / tileSize_;

    std::vector<Vector2> snakeBody = s.getBody();
    std::vector<std::vector<Vector2>> walls = g.getWalls();

    auto isOccupied = [&](int tx, int ty) -> bool {
        for (const auto& part : snakeBody) {
            int sx = static_cast<int>(part.x) / tileSize_;
            int sy = static_cast<int>(part.y) / tileSize_;
            if (sx == tx && sy == ty) {
                return true;
            }
        }
        for (const auto& wall : walls) {
            for (const auto& cell : wall) {
                if (static_cast<int>(cell.x) == tx && static_cast<int>(cell.y) == ty) {
                    return true;
                }
            }
        }
        return false;
    };

    static std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> distX(0, cols - 1);
    std::uniform_int_distribution<> distY(0, rows - 1);

    const int maxAttempts = cols * rows;
    for (int attempt = 0; attempt < maxAttempts; ++attempt) {
        int tx = distX(gen);
        int ty = distY(gen);
        if (!isOccupied(tx, ty)) {
            return { static_cast<float>(tx * tileSize_), static_cast<float>(ty * tileSize_) };
        }
    }

    return pos_;
}
```

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

**A* deeper dive**  
The algorithm is standard A* with a priority queue ordered by `f = g + h`:

- `g` is the cost from the start (path length so far).
- `h` is the Manhattan heuristic to the goal.
- `f` is the estimated total cost.

Blocked tiles are constructed from both the wall layout and the snake body (excluding the head). This makes the path search respect obstacles and avoids impossible routes.

```cpp
// blocked[y][x] marks tiles the pathfinder is not allowed to enter.
std::vector<std::vector<bool>> blocked(rows, std::vector<bool>(cols, false));

for (const auto& wall : g.getWalls()) {
    for (const auto& cell : wall) {
        int wx = static_cast<int>(cell.x);
        int wy = static_cast<int>(cell.y);
        if (wx >= 0 && wx < cols && wy >= 0 && wy < rows) {
            blocked[wy][wx] = true;
        }
    }
}

// Treat the snake body as obstacles, but allow the head tile as the start.
std::vector<Vector2> body = s.getBody();
for (int i = 1; i < static_cast<int>(body.size()); ++i) {
    int bx = static_cast<int>(body[i].x) / tileSize;
    int by = static_cast<int>(body[i].y) / tileSize;
    if (bx >= 0 && bx < cols && by >= 0 && by < rows) {
        blocked[by][bx] = true;
    }
}
```

The search expands the tile with the lowest `f` each step, records parents in `cameFrom`, and reconstructs the path by walking backward from the goal:

```cpp
while (!(cx == startX && cy == startY)) {
    path.push_back({ static_cast<float>(cx), static_cast<float>(cy) });
    Vector2 prev = cameFrom[cy][cx];
    if (prev.x < 0) {
        break;
    }
    cx = static_cast<int>(prev.x);
    cy = static_cast<int>(prev.y);
}
std::reverse(path.begin(), path.end());
```

Because the returned path is in **tile coordinates**, it can be drawn directly as grid-aligned rectangles. This also keeps the logic consistent with the grid-based movement used elsewhere in the project.

I also used `https://chatgpt.com/` to help clarify the A* algorithm and sanity-check the approach while implementing it.

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
![Figure 1: Gameplay screenshot](assets/game.png)

[Figure 2: Gameplay video showing growth and A* path toggle](https://youtu.be/hw-p8oOroXE)

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
The first major issue was **obstacle-aware spawning**. Early on, food occasionally appeared inside the snake or inside the wall clusters. This made the game feel unfair and also exposed that my collision logic wasn’t fully aligned with how I was representing positions. The fix was to treat the grid as the source of truth: I added a check that converts both snake segments and wall tiles into the same tile coordinate space before confirming a spawn. This forced me to be explicit about the difference between **pixel coordinates** (for drawing) and **tile coordinates** (for game logic). Once that pipeline was clean, the food placement was reliable and the gameplay felt more consistent.

The second big challenge was **pathfinding correctness**. Implementing A* is straightforward on paper, but getting the details right in a running game required careful thought. The snake’s body needed to be treated as obstacles while still allowing the head tile to act as the starting node. If I treated the full body as blocked, the algorithm could never start. If I treated the whole snake as free, the path would route straight through itself. The final approach was to block every body tile except the head, and to build the `blocked` grid from both the snake and the walls. That made the search result match what the player expects visually.

The third issue was **coordinate conversion drift**. At several points I mixed up tile space and pixel space, which created off‑by‑one movement or incorrect collision checks. To fix this, I made sure any comparison between objects first normalized into tile space (by dividing by `tileSize`). This eliminated a class of small, frustrating bugs and made the overall system easier to reason about.

### 3.2 Learning Outcomes
The biggest learning outcome was the value of **grid‑based reasoning** in game logic. By enforcing tile‑aligned movement, the snake became predictable, collision tests became simpler, and the A* search could operate on a clean discrete space. This was a direct upgrade from my earlier first‑year tic‑tac‑toe project, where I only used a 2D array for text output. Here, the same conceptual grid became the foundation for graphics, movement, AI, and collision.

Implementing **A*** in a real-time visual context also deepened my understanding of search algorithms. In theory, A* is about prioritizing nodes by a cost + heuristic estimate. In practice, I had to choose a heuristic that fits the movement constraints (Manhattan distance for 4‑direction tiles), handle blocked tiles properly, and return a path that could be visualized in a way that made sense to the player. The visual feedback from the pause‑mode path overlay was especially valuable for debugging and verifying that the algorithm was doing what I intended.

Finally, structuring the code into `Snake`, `Food`, `Grid`, and `AStar` gave me a more maintainable design than a single monolithic file. It made the project easier to extend and easier to explain. Even though the project is small, the separation of concerns helped keep my mental model clear while testing and debugging.

### 3.3 What I Would Do Differently
If I were to restart this project, I would build a **game state system** from the beginning. Right now the game ends by closing the window, which is functional but abrupt. A simple state machine (menu → playing → paused → game over) would make the experience cleaner and would create a natural place for UI features like a score display, restart button, and difficulty selection.

I would also add **progressive difficulty**. A classic Snake game becomes more engaging as speed increases or as obstacles shift. With the current structure, this could be added by modifying the movement delay over time and randomizing wall layouts between rounds. That would also make the A* visualization more interesting because the search space would change, forcing different routes.

Finally, I would polish **input and feedback**: add input buffering so fast key presses are not missed, add sound effects for collisions and food collection, and add a visible score counter. These features are not hard to implement, but they make the project feel more complete and professional.

### 3.4 Motivation (Why This Project)
I chose this project because it connected directly to earlier coursework. In first year, I built a tic‑tac‑toe game using a 2D array and ASCII output. That project taught me how to navigate and reason about a grid, but it was entirely text‑based. For this assignment I wanted to revisit the same idea, but apply stronger C++ skills and proper visuals.

Although graphics were not required, I had been experimenting with **raylib** in my spare time and found it approachable and productive. It let me focus on the game logic rather than complex rendering setup. A Snake game felt like the perfect fit: it is grid‑based, simple to learn, and provides a clear visual demonstration of pathfinding. The A* overlay gives a direct, visible link between algorithm and gameplay, which made the project both fun to build and useful as a learning tool.

Overall, the project sits at the intersection of my interest in game development and my interest in algorithms. It was a chance to take a familiar concept, apply it in a more advanced setting, and produce something I could both play and explain with confidence.

---

## 4. References
List external sources you used here. If none, say so.

Example:
- raylib documentation.
- A* pathfinding references (if used).

References used:
- `https://www.raylib.com/cheatsheet/cheatsheet.html`
- `https://www.geeksforgeeks.org/dsa/a-search-algorithm/`
- `https://en.cppreference.com/w/`
- `https://chatgpt.com/` (used only for help with the A* algorithm)

