//
// Created by zero on 26/10/2020.
//

#pragma once

#include <array>
#include <vector>

#include <SFML/Graphics.hpp>

#include "matrices.hpp"
#include "conversion.hpp"


sf::Color operator*(float f, sf::Color c);
sf::Color operator/(sf::Color c, float f);


/// Point with float (x, y, z) values
class Point;

/// Triangle
class Triangle;

/// Figure, consists of triangles
class Figure;


#include "figure.inl"