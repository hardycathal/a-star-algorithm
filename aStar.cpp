/*
  _,-=._              /|_/|
  `-.}   `=._,.-=-._.{  0 0._,
     `._ _,-,          _,.-'
Cathal Hardy   '.m-"^m`m'
2026
*/

#include "aStar.h"
#include <queue>
#include <limits>

struct Node {
    int x;
    int y;
    float f;
    float g;
};

struct NodeCompare {
    bool operator()(const Node& a, const Node& b) const {
        return a.f > b.f;
    }
};

static float Heuristic(int x, int y, int gx, int gy) {
    // Manhattan distance matches 4-direction grid movement.
    return static_cast<float>(abs(gx - x) + abs(gy - y));
}

std::vector<Vector2> AStar::FindPath(const Grid& g, const Snake& s, Vector2 foodPos, int windowWidth, int windowHeight)
{
    const int tileSize = g.getTileSize();
    const int cols = windowWidth / tileSize;
    const int rows = windowHeight / tileSize;

    Vector2 headPos = s.getBody().front();
    const int startX = static_cast<int>(headPos.x) / tileSize;
    const int startY = static_cast<int>(headPos.y) / tileSize;
    const int goalX = static_cast<int>(foodPos.x) / tileSize;
    const int goalY = static_cast<int>(foodPos.y) / tileSize;

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

    if (startX == goalX && startY == goalY) {
        return {};
    }

    const float INF = std::numeric_limits<float>::infinity();
    std::vector<std::vector<float>> gScore(rows, std::vector<float>(cols, INF));
    std::vector<std::vector<Vector2>> cameFrom(rows, std::vector<Vector2>(cols, { -1, -1 }));
    std::vector<std::vector<bool>> closed(rows, std::vector<bool>(cols, false));

    // Priority queue always expands the tile with the lowest estimated total cost first.
    std::priority_queue<Node, std::vector<Node>, NodeCompare> open;
    gScore[startY][startX] = 0.0f;
    open.push({ startX, startY, Heuristic(startX, startY, goalX, goalY), 0.0f });

    const int dirs[4][2] = { {1,0}, {-1,0}, {0,1}, {0,-1} };

    while (!open.empty()) {
        Node current = open.top();
        open.pop();

        // Skip queue entries for tiles we already finished with a better path.
        if (closed[current.y][current.x]) {
            continue;
        }
        closed[current.y][current.x] = true;

        if (current.x == goalX && current.y == goalY) {
            std::vector<Vector2> path;
            int cx = goalX;
            int cy = goalY;

            // Walk backward from the goal using the recorded parents, then reverse.
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
            return path;
        }

        for (int i = 0; i < 4; ++i) {
            int nx = current.x + dirs[i][0];
            int ny = current.y + dirs[i][1];
            if (nx < 0 || nx >= cols || ny < 0 || ny >= rows) {
                continue;
            }
            if (blocked[ny][nx]) {
                continue;
            }

            // Every move costs 1, so g is just the path length so far.
            float tentativeG = current.g + 1.0f;
            if (tentativeG < gScore[ny][nx]) {
                gScore[ny][nx] = tentativeG;
                cameFrom[ny][nx] = { static_cast<float>(current.x), static_cast<float>(current.y) };
                float f = tentativeG + Heuristic(nx, ny, goalX, goalY);
                open.push({ nx, ny, f, tentativeG });
            }
        }
    }

    return {};
}
