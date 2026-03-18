#include "raylib.h"
#include <vector>
#include "aStar.h"
#include "food.h"
#include "grid.h"
#include "snake.h"

int main() {
    const int windowWidth = 800, windowHeight = 600, tileSize = 25;
    const double moveDelay = 0.2;
    const double pathStepDelay = 0.05;

    InitWindow(windowWidth, windowHeight, "Snake");
    SetTargetFPS(200);

    Grid g(
        tileSize,
        std::vector<std::vector<int>>(windowHeight / tileSize, std::vector<int>(windowWidth / tileSize, 0)),
        { 0, 102, 51, 255 },
        { 0, 153, 76, 255 }
    );
    Snake s(RAYWHITE, 0);
    Food f(tileSize, RED, windowWidth, windowHeight, g, s);
    double lastMoveTime = 0.0;
    double lastPathStepTime = 0.0;
    bool paused = false;
    int visiblePathTiles = 0;
    std::vector<Vector2> path;

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_RIGHT) && s.getDir() != 0) s.setDir(1);
        if (IsKeyPressed(KEY_LEFT) && s.getDir() != 1) s.setDir(0);
        if (IsKeyPressed(KEY_UP) && s.getDir() != 3) s.setDir(2);
        if (IsKeyPressed(KEY_DOWN) && s.getDir() != 2) s.setDir(3);
        if (IsKeyPressed(KEY_SPACE)) s.addPart();
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
        if (!paused) f.detectCollisionAndMove(s, g, windowWidth, windowHeight);
        if (paused && visiblePathTiles < static_cast<int>(path.size()) && GetTime() - lastPathStepTime >= pathStepDelay) {
            ++visiblePathTiles;
            lastPathStepTime = GetTime();
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);
        s.boundaryCheck(g);
        g.drawGrid();
        s.drawSnake();
        f.drawFood();
        g.drawWalls();

        if (paused) {
            for (int i = 0; i < visiblePathTiles; ++i) {
                const auto& tile = path[i];
                DrawRectangle(tile.x * tileSize, tile.y * tileSize, tileSize, tileSize, YELLOW);
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
