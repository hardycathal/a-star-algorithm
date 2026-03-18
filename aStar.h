#ifndef ASTAR_H
#define ASTAR_H

#include "raylib.h"
#include <vector>
#include "grid.h"
#include "snake.h"

class AStar {
private:

public:
    // Finds a shortest tile-by-tile path from the snake head to the food.
    // The returned path contains tile coordinates, not pixel coordinates.
    // The snake's current tile is excluded, so the first entry is the next move.
    static std::vector<Vector2> FindPath(const Grid&, const Snake&, Vector2 foodPos, int windowWidth, int windowHeight);
};

#endif // ASTAR_H
