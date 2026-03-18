#ifndef SNAKE_H
#define SNAKE_H

#include "raylib.h"
#include <vector>
#include "grid.h"

class Snake {
private:
	std::vector<Vector2> body_; // Vector 2, to hold x, y postion of snake's body
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

#endif // SNAKE_H
