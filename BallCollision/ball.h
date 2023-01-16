#pragma once

#include "SFML/Graphics.hpp"

class Ball
{
public:
    Ball() {}
    Ball(const sf::Vector2f& position,
        const sf::Vector2f& direction,
        float radius,
        float speed = 0);
    void setPosition(const sf::Vector2f& pos);
    void setDirection(const sf::Vector2f& dir);
    void setSpeed(double speed_);
    double distanceFromOrigin();
    void draw(sf::RenderWindow& window);


    double angle();
    sf::Vector2f speedProjection();

//private:
    sf::Vector2f p;
    sf::Vector2f dir;
    float r = 0;
    float speed = 0;
    float distance_from_origin = 0;
    
    sf::Vector2f p_prev;
    sf::Vector2f dir_prev;
    float speed_prev = 0;
};