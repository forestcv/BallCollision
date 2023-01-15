#include "balls_controller.h"
#include "utility.h"

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
        //ball.dir.x = -ball.dir.x;
        ball.setDirection(sf::Vector2f{ -ball.dir.x , ball.dir.y });
        if (nextPosition.x + ball.r * 2 >= fieldWidth)
        {
            if (fieldWidth - nextPosition.x > ball.r * 2)
                //ball.p.x = (fieldWidth - ball.r) * 2 - nextPosition.x;
                ball.setPosition(sf::Vector2f{ (fieldWidth - ball.r) * 2 - nextPosition.x,
                    ball.p.y });
            else
                //ball.p.x = fieldWidth - ball.r * 2;
                ball.setPosition(sf::Vector2f{ fieldWidth - ball.r * 2 , ball.p.y });
        }
        else
        {
            ball.p.x = -nextPosition.x;
        }
        isCollision = true;
    }
    if (nextPosition.y <= 0 || nextPosition.y + ball.r * 2 >= fieldHieght)
    {
        //ball.dir.y = -ball.dir.y;
        ball.setDirection(sf::Vector2f{ball.dir.x , -ball.dir.y});
        if (nextPosition.y + ball.r * 2 >= fieldHieght)
        {
            if (fieldHieght - nextPosition.y > ball.r * 2)
                //ball.p.y = (fieldHieght - ball.r) * 2 - nextPosition.y;
                ball.setPosition(sf::Vector2f{ ball.p.x, (fieldHieght - ball.r) * 2 - nextPosition.y });
            else
                //ball.p.y = fieldHieght - ball.r * 2;
                ball.setPosition(sf::Vector2f{ ball.p.x, fieldHieght - ball.r * 2 });
        }
        else
        {
            //ball.p.y = -nextPosition.y;
            ball.setPosition(sf::Vector2f{ ball.p.x, -nextPosition.y });
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
    //double alpha = pi - ball.angle();
    //ball.p.x = ball.p.x - intersection * cos(alpha);
    //ball.p.y = ball.p.y - intersection * sin(alpha);
    ball.setPosition(nextPosition(ball, -deltaTime));
    ball.setPosition(nextPosition(ball, deltaTime));
}

static bool applyBallCollision(Ball& first, Ball& second, float deltaTime)
{
    double distance = sqrt((first.p.x - second.p.x) * (first.p.x - second.p.x) +
        (first.p.y - second.p.y) * (first.p.y - second.p.y));
    double intersection = first.r + second.r - distance;
    if (intersection >= 0)
    {
        double intersectionTime = intersection / (first.speed + second.speed);
        
        /*
        if (first.r < second.r)
        {
            auto intersectionPoints = utility::findCircleLineIntersection(first.p_prev,
                first.p,
                second.p,
                second.r);
            if (intersectionPoints.empty())
            {
                return false;
            }
            sf::Vector2f intersectionPt;
            if (intersectionPoints.size() == 2)
            {
                bool isFirstOnSegment = utility::isPointOnSegment(first.p_prev, first.p, intersectionPoints[0]);
                bool isSecondOnSegment = utility::isPointOnSegment(first.p_prev, first.p, intersectionPoints[1]);
                if(isFirstOnSegment && isSecondOnSegment)
                {
                    if (utility::distance(first.p_prev, intersectionPoints[0]) <
                        utility::distance(first.p_prev, intersectionPoints[1]))
                    {
                        intersectionPt = intersectionPoints[0];
                    }
                    else
                    {
                        intersectionPt = intersectionPoints[1];
                    }
                }
                else if (isFirstOnSegment)
                {
                    intersectionPt = intersectionPoints[0];
                }
                else
                {
                    intersectionPt = intersectionPoints[1];
                }
            }
            else
            {
                intersectionPt = intersectionPoints[0];
            }
            intersectionTime = utility::distance(intersectionPt, first.p);
        }
        else
        {
            auto intersectionPoints = utility::findCircleLineIntersection(second.p_prev,
                second.p,
                first.p,
                first.r);
            if (intersectionPoints.empty())
            {
                return false;
            }
            sf::Vector2f intersectionPt;
            if (intersectionPoints.size() == 2)
            {
                bool isFirstOnSegment = utility::isPointOnSegment(second.p_prev, second.p, intersectionPoints[0]);
                bool isSecondOnSegment = utility::isPointOnSegment(second.p_prev, second.p, intersectionPoints[1]);
                if (isFirstOnSegment && isSecondOnSegment)
                {
                    if (utility::distance(second.p_prev, intersectionPoints[0]) <
                        utility::distance(second.p_prev, intersectionPoints[1]))
                    {
                        intersectionPt = intersectionPoints[0];
                    }
                    else
                    {
                        intersectionPt = intersectionPoints[1];
                    }
                }
                else if (isFirstOnSegment)
                {
                    intersectionPt = intersectionPoints[0];
                }
                else
                {
                    intersectionPt = intersectionPoints[1];
                }
            }
            else
            {
                intersectionPt = intersectionPoints[0];
            }
            intersectionTime = utility::distance(intersectionPt, second.p);
        }
        */

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


