#pragma once

#include <SFML/System.hpp>

namespace TracerX
{

sf::Vector3f cross(const sf::Vector3f& a, const sf::Vector3f& b);

sf::Vector3f mult(const sf::Vector3f& a, const sf::Vector3f& b);

sf::Vector3f div(const sf::Vector3f& a, const sf::Vector3f& b);

float dot(const sf::Vector3f& a, const sf::Vector3f& b);

sf::Vector3f normalized(const sf::Vector3f& v);

sf::Vector3f rotateAroundAxis(const sf::Vector3f& v, const sf::Vector3f& axis, float angle);

sf::Vector3f rotateX(const sf::Vector3f& v, float angle);

sf::Vector3f rotateY(const sf::Vector3f& v, float angle);

sf::Vector3f rotateZ(const sf::Vector3f& v, float angle);

float length(const sf::Vector3f& v);

sf::Vector3f min(const sf::Vector3f& a, const sf::Vector3f& b);

sf::Vector3f max(const sf::Vector3f& a, const sf::Vector3f& b);

}