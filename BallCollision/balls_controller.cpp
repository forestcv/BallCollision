#include "balls_controller.h"

static sf::Vector2f nextPosition(const Ball &ball, float deltaTime)
{
    sf::Vector2f nextPos;
    float dx = ball.dir.x * ball.speed * deltaTime;
    float dy = ball.dir.y * ball.speed * deltaTime;
    nextPos.x = ball.p.x + dx;
    nextPos.y = ball.p.y + dy;
    return nextPos;
}

static bool applyWallCollision(Ball& ball, const sf::Vector2f &nextPosition,
    int fieldWidth, int fieldHieght)
{
    bool isCollision = false;
    if (nextPosition.x <= 0 || nextPosition.x + ball.r * 2 >= fieldWidth)
    {
        ball.dir.x = -ball.dir.x;
        ball.p.y -= (nextPosition.y - ball.p.y) / 2;
        isCollision = true;
    }
    if (nextPosition.y <= 0 || nextPosition.y + ball.r * 2 >= fieldHieght)
    {
        ball.dir.y = -ball.dir.y;
        ball.p.x -= (nextPosition.x - ball.p.x) / 2;
        isCollision = true;
    }
    if(!isCollision)
    {
        ball.setPosition(nextPosition);
    }
    return isCollision;
}

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
        applyWallCollision(ball, nextPosition(ball, deltaTime), fieldWidth, fieldHieght);
        //ball.setPosition(nextPosition(ball, deltaTime));
    }
}


