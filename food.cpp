#include "food.h"
#include <random>
#include "grid.h"
#include "snake.h"

Food::Food(int t, Color c, int a, int b, const Grid& g, const Snake& s) : tileSize_{ t }, colour_{ c }
{
    Vector2 random = getRandom(g, s, a, b);
    pos_.x = random.x;
    pos_.y = random.y;
}

void Food::drawFood() {

    DrawRectangle(pos_.x, pos_.y, tileSize_, tileSize_, colour_);
}

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
