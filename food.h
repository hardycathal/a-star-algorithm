#ifndef FOOD_H
#define FOOD_H

#include "raylib.h"
#include <vector>
#include "snake.h"
#include "grid.h"


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

#endif // FOOD_H
