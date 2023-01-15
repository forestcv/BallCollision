#pragma once

#include <vector>

#include "SFML/Graphics.hpp"

namespace utility
{
	std::vector<double> solveQuadraticEquation(double a,
		double b, 
		double c);
	std::vector<sf::Vector2f> findCircleLineIntersection(
		const sf::Vector2f& firstLinePt,
		const sf::Vector2f& secondLinePt,
		const sf::Vector2f& circle,
		double radius
	);
	double distance(const sf::Vector2f& first,
		const sf::Vector2f& second);
	bool isPointOnSegment(const sf::Vector2f& firstLinePt,
		const sf::Vector2f& secondLinePt,
		const sf::Vector2f& point);
};


