#pragma once

#include <SFML/System.hpp>

namespace TracerX
{

struct Sphere
{
    alignas(16) sf::Vector3f origin;
    float radius;
    int materialId;

    Sphere(sf::Vector3f origin, float radius);

    Sphere(sf::Vector3f origin, float radius, int materialId);
};

}