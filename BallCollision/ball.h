#pragma once

#include "SFML/Graphics.hpp"

class Ball
{
public:
    Ball(const sf::Vector2f& position,
        const sf::Vector2f& direction,
        float radius,
        float speed = 0);
    void move(double deltaTime);
    void move(double speed, double deltaTime);
    double speed() const ;
    const sf::Vector2f& position() const;
    const sf::Vector2f& direction() const;
    double previousSpeed() const;
    const sf::Vector2f& previousPosition() const;
    const sf::Vector2f& previousDirection() const;
    void setPosition(const sf::Vector2f& pos);
    void setDirection(const sf::Vector2f& dir);
    void setSpeed(double speed_);
    double distanceFromOrigin() const;
    void draw(sf::RenderWindow& window) const;


    double angle() const ;
    sf::Vector2f speedProjection() const;

    struct State
    {
        sf::Vector2f pos;
        sf::Vector2f dir;
        float speed = 0;
    };

    float r = 0;

private:
    State currentState;
    State prevState;

    float distance_from_origin = 0;
};