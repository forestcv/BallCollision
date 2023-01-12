#include "ball.h"

Ball::Ball(const sf::Vector2f& position, const sf::Vector2f& direction,
    float radius, float speed) :
    p(position), dir(direction), r(radius), speed(speed)
{

}

void Ball::setPosition(const sf::Vector2f& pos)
{
    p = pos;
}

void Ball::draw(sf::RenderWindow& window)
{
    sf::CircleShape gball;
    gball.setRadius(r);
    gball.setPosition(p.x, p.y);
    window.draw(gball);
}