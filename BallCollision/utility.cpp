#include "utility.h"

std::vector<double> utility::solveQuadraticEquation(double a, double b, double c)
{
    std::vector<double> solution;
    solution.reserve(2);

    double D = b * b - 4 * a * c;
    if (D > 0)
    {        
        solution.emplace_back((-b + sqrt(D)) / (2 * a));
        //solution.back().y = -(a * solution.back().x + c) / b;
        solution.emplace_back((-b - sqrt(D)) / (2 * a));
        //solution.back().y = -(a * solution.back().x + c) / b;
    }
    else if (D == 0)
    {
        solution.emplace_back(-b / (2 * a));
        //solution.back().y = -(a * solution.back().x + c) / b;
    }

    solution.shrink_to_fit();
    return solution;
}

std::vector<sf::Vector2f> utility::findCircleLineIntersection(const sf::Vector2f& firstLinePt, 
    const sf::Vector2f& secondLinePt, 
    const sf::Vector2f& circle, 
    double radius)
{
    std::vector<sf::Vector2f> solution;
    solution.reserve(2);

    double A = secondLinePt.y - firstLinePt.y;
    double B = firstLinePt.x - secondLinePt.x;
    double C = secondLinePt.x * firstLinePt.y -
        firstLinePt.x * secondLinePt.y;

    sf::Vector2f center{float(circle.x + radius), float(circle.y + radius)};

    double a = A * A + B * B;
    double b = 2 * A * C + 2 * A * B * center.y - 2 * B * B * center.x;
    double c = C * C + 2 * B * C * center.y -
        B * B * (radius * radius - center.x * center.x - center.y * center.y);

    auto solutionX = solveQuadraticEquation(a, b, c);

    for (const auto& x : solutionX)
    {
        sf::Vector2f pt;
        pt.x = x;
        pt.y = -(A * x + C) / B;
        solution.push_back(pt);
    }

    solution.shrink_to_fit();
    return solution;
}

double utility::distance(const sf::Vector2f& first, const sf::Vector2f& second)
{
    return sqrt(pow(second.x - first.x, 2) + pow(second.y - first.y, 2));
}

bool utility::isPointOnSegment(const sf::Vector2f& firstLinePt, const sf::Vector2f& secondLinePt, const sf::Vector2f& point)
{
    return distance(firstLinePt, point) + distance(secondLinePt, point) - distance(firstLinePt, secondLinePt) < 0.1;
}




