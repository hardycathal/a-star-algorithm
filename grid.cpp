/*
  _,-=._              /|_/|
  `-.}   `=._,.-=-._.{  0 0._,
     `._ _,-,          _,.-'
Cathal Hardy   '.m-"^m`m'
2026
*/

#include "raylib.h"
#include <vector>
#include <iostream>
#include "grid.h"

Grid::Grid(int t,
    const std::vector<std::vector<int>>& grid,
    Color primary,
    Color accent)
{
    tileSize_ = t;
    vec_ = grid;
    pColour_ = primary;
    aColour_ = accent;
    initWalls();
}


void Grid::setTileSize(int t) {
    tileSize_ = t;
}

void Grid::setGrid(const std::vector<std::vector<int>>& g) { vec_ = g; }


void Grid::setPrimaryColour(Color p) {
    pColour_ = p;
}

void Grid::setAccentColour(Color a) {
    aColour_ = a;
}

std::vector<std::vector<int>> Grid::getGrid() const {
    return vec_;
}

Color Grid::getPrimaryColour() const {
    return pColour_;
}

Color Grid::getAccentColour() const {
    return aColour_;
}

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

void Grid::initWalls()
{
    walls_ = {
        { {4,4}, {4,5}, {4,6}, {4,7}, {4,8} },
        { {8,14}, {9,14}, {10,14}, {11,14}, {12,14} },
        { {16,4}, {16,5}, {16,6}, {16,7}, {16,8} },
        { {20,16}, {21,16}, {22,16}, {23,16}, {24,16} },
        { {26,6}, {26,7}, {26,8}, {27,8}, {28,8} },
        { {12,20}, {13,20}, {14,20}, {15,20} }
    };
}

