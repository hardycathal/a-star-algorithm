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
#include "snake.h"
#include "grid.h"

Snake::Snake(Color c, int direction)
    : body_{ { 300, 300 }, { 325, 300 }, { 350, 300 } }, colour_{ c }, dir_{ direction }
{

}

void Snake::drawSnake()
{
    for (int i = 0; i < (int)body_.size(); i++) {
        DrawRectangle(body_[i].x, body_[i].y, 25, 25, colour_);
    }

}

void Snake::addPart()
{
    body_.push_back(body_.back());
}

void Snake::setDir(int d)
{
    dir_ = d;
}

int Snake::getDir()
{
    return dir_;
}

void Snake::move()
{
    Vector2 frontPart = body_.front();

    switch (dir_) {
    case 0: // left
        frontPart.x -= 25;
        break;
    case 1: // right
        frontPart.x += 25;
        break;
    case 2: // up
        frontPart.y -= 25;
        break;
    case 3: // down
        frontPart.y += 25;
        break;
    }

    body_.insert(body_.begin(), frontPart);
    body_.pop_back();                       
}

int Snake::getSize() {
    return body_.size();
}

std::vector<Vector2> Snake::getBody() const {
    return body_;
}

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
