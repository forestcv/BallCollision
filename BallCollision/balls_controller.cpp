#include "balls_controller.h"

const double pi = 3.14159265358979323846;

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
        if (nextPosition.x + ball.r * 2 >= fieldWidth)
        {
            if(fieldWidth - nextPosition.x > ball.r * 2)
                ball.p.x = (fieldWidth - ball.r) * 2 - nextPosition.x;
            else
                ball.p.x = fieldWidth - ball.r * 2;
        }
        else
        {
            ball.p.x = -nextPosition.x;
        }
        isCollision = true;
    }
    if (nextPosition.y <= 0 || nextPosition.y + ball.r * 2 >= fieldHieght)
    {
        ball.dir.y = -ball.dir.y;
        if (nextPosition.y + ball.r * 2 >= fieldHieght)
        {
            if (fieldHieght - nextPosition.y > ball.r * 2)
                ball.p.y = (fieldHieght - ball.r) * 2 - nextPosition.y;
            else
                ball.p.y = fieldHieght - ball.r * 2;
        }
        else
        {
            ball.p.y = -nextPosition.y;
        }
        isCollision = true;
    }
    if(!isCollision)
    {
        ball.setPosition(nextPosition);
    }
    return isCollision;
}

static std::tuple<double, double> calcSpeedProjectionAfterHit(double firstSpeed,
    double secondSpeed,
    double firstMass,
    double secondMass)
{
    double massSum = firstMass + secondMass;

    double newFirstSpeed = ((firstMass - secondMass) * firstSpeed + 2 * secondMass * secondSpeed) /
        massSum;
    double newSecondSpeed = (2 * firstMass * firstSpeed + (secondMass - firstMass) * secondSpeed) /
        massSum;

    return std::tuple<double, double>(newFirstSpeed, newSecondSpeed);
}

static void ballCollisionSolver(Ball& first, Ball& second)
{
    // получение проекций скоростей до столкновения
    sf::Vector2f firstSpeed = first.speedProjection();
    sf::Vector2f secondSpeed = second.speedProjection();

    double firstSquareRadius = first.r * first.r;
    double secondSquareRadius = second.r * second.r;
    double squareRadiusSum = firstSquareRadius + secondSquareRadius;

    // расчет проекций скростей после столкновения
    auto [newFirstSpeedX, newSecondSpeedX] = calcSpeedProjectionAfterHit(firstSpeed.x,
                                                                        secondSpeed.x,
                                                                        firstSquareRadius,
                                                                        secondSquareRadius);
    auto [newFirstSpeedY, newSecondSpeedY] = calcSpeedProjectionAfterHit(firstSpeed.y,
        secondSpeed.y,
        firstSquareRadius,
        secondSquareRadius);

    first.speed = sqrt(newFirstSpeedX * newFirstSpeedX + newFirstSpeedY * newFirstSpeedY);
    second.speed = sqrt(newSecondSpeedX * newSecondSpeedX + newSecondSpeedY * newSecondSpeedY);

    first.dir.x = newFirstSpeedX / first.speed;
    first.dir.y = newFirstSpeedY / first.speed;

    second.dir.x = newSecondSpeedX / second.speed;
    second.dir.y = newSecondSpeedY / second.speed;
}

static void applyBallIntersection(Ball& ball,
    float intersection, float deltaTime)
{
    double alpha = pi - ball.angle();
    ball.p.x = ball.p.x - intersection * cos(alpha);
    ball.p.y = ball.p.y - intersection * sin(alpha);
    ball.p = nextPosition(ball, deltaTime);
}

static bool applyBallCollision(Ball& first, Ball& second, float deltaTime)
{
    double distance = sqrt((first.p.x - second.p.x) * (first.p.x - second.p.x) +
        (first.p.y - second.p.y) * (first.p.y - second.p.y));
    double intersection = first.r + second.r - distance;
    if (intersection >= 0)
    {
        double intersectionTime = intersection / first.speed;

        ballCollisionSolver(first, second);

        applyBallIntersection(first, intersection, intersectionTime);
        applyBallIntersection(second, intersection, intersectionTime);

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
            applyBallCollision(balls->operator[](i), balls->operator[](j), deltaTime);
        }
    }
}


