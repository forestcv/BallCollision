#include "ball.h"
#include "utility.h"

Ball::Ball(const sf::Vector2f& position, const sf::Vector2f& direction,
    float radius, float speed) :
    r(radius)
{
    currentState.pos = position;
    currentState.dir = direction;
    currentState.speed = speed;
    prevState = currentState;
    distance_from_origin = utility::distance(sf::Vector2f(0, 0), currentState.pos);
}

void Ball::move(double deltaTime)
{;
    float dx = currentState.dir.x * currentState.speed * deltaTime;
    float dy = currentState.dir.y * currentState.speed * deltaTime;
    setPosition(currentState.pos + sf::Vector2f(dx, dy));
}

void Ball::move(double speed, double deltaTime)
{
    float dx = currentState.dir.x * speed * deltaTime;
    float dy = currentState.dir.y * speed * deltaTime;
    setPosition(currentState.pos + sf::Vector2f(dx, dy));
}

double Ball::speed() const
{
    return currentState.speed;
}

const sf::Vector2f& Ball::position() const
{
    return currentState.pos;
}

const sf::Vector2f& Ball::direction() const
{
    return currentState.dir;
}

double Ball::previousSpeed() const
{
    return prevState.speed;
}

const sf::Vector2f& Ball::previousPosition() const
{
    return prevState.pos;
}

const sf::Vector2f& Ball::previousDirection() const
{
    return prevState.dir;
}

void Ball::setPosition(const sf::Vector2f& pos)
{
    prevState.pos = currentState.pos;
    currentState.pos = pos;
    distance_from_origin = utility::distance(sf::Vector2f(0, 0), currentState.pos);
}

void Ball::setDirection(const sf::Vector2f& direction)
{
    prevState.dir = currentState.dir;
    currentState.dir = direction;
}

void Ball::setSpeed(double speed) 
{
    prevState.speed = currentState.speed;
    currentState.speed = speed;
}

double Ball::distanceFromOrigin() const
{
    return distance_from_origin;
}

void Ball::draw(sf::RenderWindow& window) const
{
    sf::CircleShape gball;
    gball.setRadius(r);
    gball.setPosition(currentState.pos.x, currentState.pos.y);
    window.draw(gball);
}

double Ball::angle() const
{
    return atan2(currentState.dir.y, currentState.dir.x);
}

sf::Vector2f Ball::speedProjection() const
{
    sf::Vector2f speed;
    double alpha = angle();
    speed.x = currentState.speed * cos(alpha);
    speed.y = currentState.speed * sin(alpha);
    return speed;
}
