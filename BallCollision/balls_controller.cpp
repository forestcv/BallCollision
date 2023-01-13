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
        if (nextPosition.x + ball.r * 2 >= fieldWidth)
            //ball.p.x = fieldWidth - ball.r * 2;
            ball.p.x = (fieldWidth - ball.r * 2) * 2 - nextPosition.x;
        isCollision = true;
    }
    if (nextPosition.y <= 0 || nextPosition.y + ball.r * 2 >= fieldHieght)
    {
        ball.dir.y = -ball.dir.y;
        ball.p.x -= (nextPosition.x - ball.p.x) / 2;
        if (nextPosition.y + ball.r * 2 >= fieldHieght)
            ball.p.y = (fieldHieght - ball.r * 2) * 2 - nextPosition.y;
        isCollision = true;
    }
    if(!isCollision)
    {
        ball.setPosition(nextPosition);
    }
    return isCollision;
}

static bool applyBallCollision(Ball& first, Ball& second)
{
    double distance = sqrt((first.p.x - second.p.x) * (first.p.x - second.p.x) +
        (first.p.y - second.p.y) * (first.p.y - second.p.y));
    if (distance <= first.r + second.r)
    {
        first.dir.x = -first.dir.x;
        first.dir.y = -first.dir.y;
        second.dir.x = -second.dir.x;
        second.dir.y = -second.dir.y;
        return true;
    }
    return false;
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
    for (int i = 0; i < balls->size() - 1; i++)
    {
        for (int j = i + 1; j < balls->size(); j++)
        {
            applyBallCollision(balls->operator[](i), balls->operator[](j));
        }
    }
}


