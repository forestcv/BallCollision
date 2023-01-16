#pragma once

#include <memory>
#include <vector>

#include "ball.h"

using BallsPtr = std::shared_ptr<std::vector<Ball>>;

class BallsController
{
public:
	BallsController(const BallsPtr &balls, 
		int fieldWidth,
		int fieldHieght);
	void update(float deltaTime);
private:

	BallsPtr balls;
	int fieldWidth = 0;
	int fieldHieght = 0;
};

