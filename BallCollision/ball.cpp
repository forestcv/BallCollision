#include "ball.h"

Ball::Ball(const sf::Vector2f& position, const sf::Vector2f& direction,
    float radius, float speed) :
    p(position), dir(direction), r(radius), speed(speed)
{

}

void Ball::setPosition(const sf::Vector2f& pos)
{
    p_prev = p;
    p = pos;
}

void Ball::setDirection(const sf::Vector2f& direction)
{
    dir_prev = dir;
    dir = direction;
}

void Ball::draw(sf::RenderWindow& window)
{
    sf::CircleShape gball;
    gball.setRadius(r);
    gball.setPosition(p.x, p.y);
    window.draw(gball);
}

double Ball::angle()
{
    return atan2(dir.y, dir.x);
}

sf::Vector2f Ball::speedProjection()
{
    sf::Vector2f speedXY;
    double alpha = angle();
    speedXY.x = speed * cos(alpha);
    speedXY.y = speed * sin(alpha);
    return speedXY;
}
