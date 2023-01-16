#include "balls_controller.h"
#include "utility.h"

#include <algorithm>

// количество интервалов деления времени рассчета при точном определнии
// места столкновения шаров
static const int descritisationParts = 5;

// функция-обработчик столкновений с краями экрана
static bool applyWallCollision(Ball& ball, int fieldWidth, int fieldHieght)
{
    bool isCollision = false;
    // просчет столкновения с левым и правым краями экрана
    if (ball.position().x <= 0 || (ball.position().x + ball.radius() * 2 >= fieldWidth))
    {
        ball.setDirection(sf::Vector2f{ -ball.direction().x , ball.direction().y });
        // правый край
        if (ball.position().x + ball.radius() * 2 >= fieldWidth)
                ball.setPosition(sf::Vector2f{ fieldWidth - ball.radius() * 2 , ball.previousPosition().y });
        // левый край
        else
            ball.setPosition(sf::Vector2f{ -ball.position().x, ball.position().y });
        isCollision = true;
    }
    // просчет столкновения с верхним и нижним краями экрана
    if (ball.position().y <= 0 || (ball.position().y + ball.radius() * 2 >= fieldHieght))
    {
        ball.setDirection(sf::Vector2f{ball.direction().x , -ball.direction().y});
        // нижний край
        if (ball.position().y + ball.radius() * 2 >= fieldHieght)
                ball.setPosition(sf::Vector2f{ ball.previousPosition().x, fieldHieght - ball.radius() * 2 });
        // верхний край
        else
            ball.setPosition(sf::Vector2f{ ball.previousPosition().x, -ball.position().y });
        isCollision = true;
    }
    return isCollision;
}

// расчет проекций скоростей после столкновения
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

// функция рассчета параметров движения шаров после столкновения
static void ballCollisionSolver(Ball& first, Ball& second)
{
    // получение проекций скоростей до столкновения
    sf::Vector2f firstSpeed = first.speedProjection();
    sf::Vector2f secondSpeed = second.speedProjection();

    // рассчет квадратов радиусов шаров - аналог масс
    double firstSquareRadius = first.radius() * first.radius();
    double secondSquareRadius = second.radius() * second.radius();

    // расчет проекций скростей после столкновения
    auto [newFirstSpeedX, newSecondSpeedX] = calcSpeedProjectionAfterHit(firstSpeed.x,
                                                                        secondSpeed.x,
                                                                        firstSquareRadius,
                                                                        secondSquareRadius);
    auto [newFirstSpeedY, newSecondSpeedY] = calcSpeedProjectionAfterHit(firstSpeed.y,
                                                                        secondSpeed.y,
                                                                        firstSquareRadius,
                                                                        secondSquareRadius);

    // расчет скоростей по их проекциям
    first.setSpeed(sqrt(newFirstSpeedX * newFirstSpeedX + newFirstSpeedY * newFirstSpeedY));
    second.setSpeed(sqrt(newSecondSpeedX * newSecondSpeedX + newSecondSpeedY * newSecondSpeedY));

    // расчет направления движения после удара
    first.setDirection(sf::Vector2f(newFirstSpeedX / first.speed(),
        newFirstSpeedY / first.speed()));
    second.setDirection(sf::Vector2f(newSecondSpeedX / second.speed(),
        newSecondSpeedY / second.speed()));
}

// функция, определяющая положения пространства и параметры движения шаров после столкновения
static bool applyBallCollision(Ball& first, Ball& second, float deltaTime)
{
    // расстояние между центрами шаров
    double distance = utility::distance(first.position(), second.position());
    double intersection = first.radius() + second.radius() - distance;
    if (intersection >= 0)
    {
        // уточнение места столкновения шаров
        double intersectionTime = 0;    // время движения шаров после столкновения
        // дискрета по времени для расчета места столкновения
        double dTime = deltaTime / descritisationParts; 
        while (intersection >= 0 && intersectionTime <= deltaTime)
        {
            // двигаем шары в обратном направлении
            first.move(-first.direction(), first.speed(), dTime);
            second.move(-second.direction(), second.speed(), dTime);
            intersectionTime += dTime;

            // определение текущего расстояния между шарами
            distance = utility::distance(first.position(), second.position());
            intersection = first.radius() + second.radius() - distance;
        }

        // расчет параметров шаров после столкновения
        ballCollisionSolver(first, second);

        // движение шаров после столкновения
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
    // предварительная проверка и разрешение коллизий шаров
    processBallsColisions(1);
}

void BallsController::update(float deltaTime)
{
    // расчет нового положения шаров
    for (auto& ball : *balls)
    {
        ball.move(deltaTime);
        // проверка на коллизии с краями  экрана
        applyWallCollision(ball, fieldWidth, fieldHieght);
    }

    processBallsColisions(deltaTime);
}

/*
Оптимизация просчета коллизий шаров построена на том,
что предварительно шары сортируются по расстоянию от начала координат,
затем в цикле при достижении шара, расстояние которого от центра координат
больше, чем аналогиченое расстояние для текущего шара плюс его диаметр
(они точно не могут пересекаться), просчет коллизий для текущего шара прекращается.
Таким образом, продвижение по циклу обсчета коллизий шаров становится
значительно быстрее
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


