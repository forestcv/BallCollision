#include "balls_controller.h"
#include "utility.h"

#include <algorithm>

const double pi = 3.14159265358979323846;

static bool applyWallCollision(Ball& ball,
    int fieldWidth, int fieldHieght)
{
    bool isCollision = false;
    if (ball.position().x <= 0 || (ball.position().x + ball.r * 2 >= fieldWidth))
    {
        //ball.dir.x = -ball.dir.x;
        ball.setDirection(sf::Vector2f{ -ball.direction().x , ball.direction().y });
        if (ball.position().x + ball.r * 2 >= fieldWidth)
        {
            if (fieldWidth - ball.position().x > ball.r * 2)
                //ball.p.x = (fieldWidth - ball.r) * 2 - nextPosition.x;
                ball.setPosition(sf::Vector2f{ (fieldWidth - ball.r) * 2 - ball.position().x,
                    ball.previousPosition().y });
            else
                //ball.p.x = fieldWidth - ball.r * 2;
                ball.setPosition(sf::Vector2f{ fieldWidth - ball.r * 2 , ball.previousPosition().y });
        }
        else
        {
            ball.setPosition(sf::Vector2f{ -ball.position().x, ball.position().y });
        }
        isCollision = true;
    }
    if (ball.position().y <= 0 || (ball.position().y + ball.r * 2 >= fieldHieght))
    {
        //ball.dir.y = -ball.dir.y;
        ball.setDirection(sf::Vector2f{ball.direction().x , -ball.direction().y});
        if (ball.position().y + ball.r * 2 >= fieldHieght)
        {
            if (fieldHieght - ball.position().y > ball.r * 2)
                //ball.p.y = (fieldHieght - ball.r) * 2 - nextPosition.y;
                ball.setPosition(sf::Vector2f{ ball.previousPosition().x, (fieldHieght - ball.r) * 2 - ball.position().y });
            else
                //ball.p.y = fieldHieght - ball.r * 2;
                ball.setPosition(sf::Vector2f{ ball.previousPosition().x, fieldHieght - ball.r * 2 });
        }
        else
        {
            //ball.p.y = -nextPosition.y;
            ball.setPosition(sf::Vector2f{ ball.previousPosition().x, -ball.position().y });
        }
        isCollision = true;
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

    // расчет проекций скростей после столкновения
    auto [newFirstSpeedX, newSecondSpeedX] = calcSpeedProjectionAfterHit(firstSpeed.x,
                                                                        secondSpeed.x,
                                                                        firstSquareRadius,
                                                                        secondSquareRadius);
    auto [newFirstSpeedY, newSecondSpeedY] = calcSpeedProjectionAfterHit(firstSpeed.y,
                                                                        secondSpeed.y,
                                                                        firstSquareRadius,
                                                                        secondSquareRadius);

    first.setSpeed(sqrt(newFirstSpeedX * newFirstSpeedX + newFirstSpeedY * newFirstSpeedY));
    second.setSpeed(sqrt(newSecondSpeedX * newSecondSpeedX + newSecondSpeedY * newSecondSpeedY));

    first.setDirection(sf::Vector2f(newFirstSpeedX / first.speed(),
        newFirstSpeedY / first.speed()));
    second.setDirection(sf::Vector2f(newSecondSpeedX / second.speed(),
        newSecondSpeedY / second.speed()));
}

static void applyBallIntersection(Ball& ball, float deltaTime)
{
    ball.move(ball.previousSpeed(), -deltaTime);
    ball.move(deltaTime);
}

static bool applyBallCollision(Ball& first, Ball& second, float deltaTime)
{
    double distance = utility::distance(first.position(), second.position());
    double intersection = first.r + second.r - distance;
    if (intersection >= 0)
    {
        double intersectionTime = intersection / (first.speed() + second.speed());

        ballCollisionSolver(first, second);

        applyBallIntersection(first, intersectionTime);
        applyBallIntersection(second, intersectionTime);

        return true;
    }
    return false;
}

BallsController::BallsController(const BallsPtr& balls,
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
        applyWallCollision(ball, fieldWidth, fieldHieght);
    }

    std::sort(balls->begin(), balls->end(), 
        [](const Ball& first, const Ball& second) 
        {
            return first.distanceFromOrigin() < second.distanceFromOrigin();
        });

    for (int i = 0; i < balls->size() - 1; i++)
    {
        for (int j = i + 1; j < balls->size(); j++)
        {
            if (balls->operator[](j).distanceFromOrigin() - 
                balls->operator[](i).distanceFromOrigin() <=
                balls->operator[](i).r)
                applyBallCollision(balls->operator[](i), 
                    balls->operator[](j), 
                    deltaTime);
            else
                break;
        }
    }
}


