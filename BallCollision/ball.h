#pragma once

#include "SFML/Graphics.hpp"

class Ball
{
public:
    Ball(const sf::Vector2f& position,
        const sf::Vector2f& direction,
        float radius,
        float speed = 0);
    void move(float deltaTime);
    void draw(sf::RenderWindow& window);
    friend class BallsController;

private:
    sf::Vector2f p;
    sf::Vector2f dir;
    float r = 0;
    float speed = 0;
};