#ifndef GRID_H
#define GRID_H

#include "raylib.h"
#include <vector>

class Grid {
private:
    int tileSize_;
    std::vector<std::vector<int>> vec_;
    Color pColour_;
    Color aColour_;
    std::vector<std::vector<Vector2>> walls_;
    void initWalls();
public:
    // Contsructor
    Grid(int tileSize,
        const std::vector<std::vector<int>>& grid,
        Color primary,
        Color accent);

    void setTileSize(int t);
    void setGrid(const std::vector<std::vector<int>>& g);
    void setPrimaryColour(Color c);
    void setAccentColour(Color a);
    void drawGrid(); // Draw Grid to screen function

    std::vector<std::vector<int>> getGrid() const;
    int getTileSize() const { return tileSize_; }
    Color getPrimaryColour() const;
    Color getAccentColour() const;
    void drawWalls();
    std::vector<std::vector<Vector2>> getWalls() const { return walls_; }
};

#endif // GRID_H
