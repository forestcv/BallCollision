#pragma once

#include <memory>
#include <vector>

#include "ball.h"

using BallsPtr = std::shared_ptr<std::vector<Ball>>;

class BallsController
{
public:
	BallsController(BallsPtr balls, 
		int fieldWidth,
		int fieldHieght);
	void update(float deltaTime);
private:
	void applyWallCollision();

	BallsPtr balls;
	int fieldWidth = 0;
	int fieldHieght = 0;
};

