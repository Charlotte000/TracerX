#pragma once

#include <SFML/System.hpp>

namespace TracerX
{

sf::Vector3f cross(sf::Vector3f a, sf::Vector3f b);

sf::Vector3f mult(sf::Vector3f a, sf::Vector3f b);

float dot(sf::Vector3f a, sf::Vector3f b);

sf::Vector3f normalized(sf::Vector3f v);

sf::Vector3f rotateAroundAxis(sf::Vector3f v, sf::Vector3f axis, float angle);

sf::Vector3f rotateX(sf::Vector3f v, float angle);

sf::Vector3f rotateY(sf::Vector3f v, float angle);

sf::Vector3f rotateZ(sf::Vector3f v, float angle);

float length(sf::Vector3f v);

sf::Vector3f min(sf::Vector3f a, sf::Vector3f b);

sf::Vector3f max(sf::Vector3f a, sf::Vector3f b);

}