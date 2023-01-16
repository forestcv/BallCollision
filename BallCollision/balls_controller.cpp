#include "balls_controller.h"
#include "utility.h"

#include <algorithm>

// ���������� ���������� ������� ������� �������� ��� ������ ����������
// ����� ������������ �����
static const int descritisationParts = 5;

// �������-���������� ������������ � ������ ������
static bool applyWallCollision(Ball& ball, int fieldWidth, int fieldHieght)
{
    bool isCollision = false;
    // ������� ������������ � ����� � ������ ������ ������
    if (ball.position().x <= 0 || (ball.position().x + ball.radius() * 2 >= fieldWidth))
    {
        ball.setDirection(sf::Vector2f{ -ball.direction().x , ball.direction().y });
        // ������ ����
        if (ball.position().x + ball.radius() * 2 >= fieldWidth)
                ball.setPosition(sf::Vector2f{ fieldWidth - ball.radius() * 2 , ball.previousPosition().y });
        // ����� ����
        else
            ball.setPosition(sf::Vector2f{ -ball.position().x, ball.position().y });
        isCollision = true;
    }
    // ������� ������������ � ������� � ������ ������ ������
    if (ball.position().y <= 0 || (ball.position().y + ball.radius() * 2 >= fieldHieght))
    {
        ball.setDirection(sf::Vector2f{ball.direction().x , -ball.direction().y});
        // ������ ����
        if (ball.position().y + ball.radius() * 2 >= fieldHieght)
                ball.setPosition(sf::Vector2f{ ball.previousPosition().x, fieldHieght - ball.radius() * 2 });
        // ������� ����
        else
            ball.setPosition(sf::Vector2f{ ball.previousPosition().x, -ball.position().y });
        isCollision = true;
    }
    return isCollision;
}

// ������ �������� ��������� ����� ������������
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

// ������� �������� ���������� �������� ����� ����� ������������
static void ballCollisionSolver(Ball& first, Ball& second)
{
    // ��������� �������� ��������� �� ������������
    sf::Vector2f firstSpeed = first.speedProjection();
    sf::Vector2f secondSpeed = second.speedProjection();

    // ������� ��������� �������� ����� - ������ ����
    double firstSquareRadius = first.radius() * first.radius();
    double secondSquareRadius = second.radius() * second.radius();

    // ������ �������� �������� ����� ������������
    auto [newFirstSpeedX, newSecondSpeedX] = calcSpeedProjectionAfterHit(firstSpeed.x,
                                                                        secondSpeed.x,
                                                                        firstSquareRadius,
                                                                        secondSquareRadius);
    auto [newFirstSpeedY, newSecondSpeedY] = calcSpeedProjectionAfterHit(firstSpeed.y,
                                                                        secondSpeed.y,
                                                                        firstSquareRadius,
                                                                        secondSquareRadius);

    // ������ ��������� �� �� ���������
    first.setSpeed(sqrt(newFirstSpeedX * newFirstSpeedX + newFirstSpeedY * newFirstSpeedY));
    second.setSpeed(sqrt(newSecondSpeedX * newSecondSpeedX + newSecondSpeedY * newSecondSpeedY));

    // ������ ����������� �������� ����� �����
    first.setDirection(sf::Vector2f(newFirstSpeedX / first.speed(),
        newFirstSpeedY / first.speed()));
    second.setDirection(sf::Vector2f(newSecondSpeedX / second.speed(),
        newSecondSpeedY / second.speed()));
}

// �������, ������������ ��������� ������������ � ��������� �������� ����� ����� ������������
static bool applyBallCollision(Ball& first, Ball& second, float deltaTime)
{
    // ���������� ����� �������� �����
    double distance = utility::distance(first.position(), second.position());
    double intersection = first.radius() + second.radius() - distance;
    if (intersection >= 0)
    {
        // ��������� ����� ������������ �����
        double intersectionTime = 0;    // ����� �������� ����� ����� ������������
        // �������� �� ������� ��� ������� ����� ������������
        double dTime = deltaTime / descritisationParts; 
        while (intersection >= 0 && intersectionTime <= deltaTime)
        {
            // ������� ���� � �������� �����������
            first.move(-first.direction(), first.speed(), dTime);
            second.move(-second.direction(), second.speed(), dTime);
            intersectionTime += dTime;

            // ����������� �������� ���������� ����� ������
            distance = utility::distance(first.position(), second.position());
            intersection = first.radius() + second.radius() - distance;
        }

        // ������ ���������� ����� ����� ������������
        ballCollisionSolver(first, second);

        // �������� ����� ����� ������������
        first.move(intersectionTime);
        second.move(intersectionTime);

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
    // ��������������� �������� � ���������� �������� �����
    processBallsColisions(1);
}

void BallsController::update(float deltaTime)
{
    // ������ ������ ��������� �����
    for (auto& ball : *balls)
    {
        ball.move(deltaTime);
        // �������� �� �������� � ������  ������
        applyWallCollision(ball, fieldWidth, fieldHieght);
    }

    processBallsColisions(deltaTime);
}

/*
����������� �������� �������� ����� ��������� �� ���,
��� �������������� ���� ����������� �� ���������� �� ������ ���������,
����� � ����� ��� ���������� ����, ���������� �������� �� ������ ���������
������, ��� ������������ ���������� ��� �������� ���� ���� ��� �������
(��� ����� �� ����� ������������), ������� �������� ��� �������� ���� ������������.
����� �������, ����������� �� ����� ������� �������� ����� ����������
����������� �������
*/
void BallsController::processBallsColisions(float deltaTime)
{
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
                balls->operator[](i).radius())
                applyBallCollision(balls->operator[](i),
                    balls->operator[](j),
                    deltaTime);
            else
                break;
        }
    }
}


