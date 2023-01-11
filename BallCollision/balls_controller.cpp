#include "balls_controller.h"

BallsController::BallsController(BallsPtr balls,
    int fieldWidth,
    int fieldHieght) :
	balls(balls), 
    fieldWidth(fieldWidth),
    fieldHieght(fieldHieght)
{

}

void BallsController::update(float deltaTime)
{
    for (auto& ball : *balls)
    {
        ball.move(deltaTime);
    }
}

void BallsController::applyWallCollision()
{
    for (auto& ball : *balls)
    {
        if(ball.p.x 
    }
}
