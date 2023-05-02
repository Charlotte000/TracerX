#pragma once

#include <SFML/Graphics.hpp>

namespace TracerX
{

struct Camera
{
    sf::Vector3f up;
    sf::Vector3f forward;
    sf::Vector3f position;
    float focalLength;
    float focusStrength;

    Camera(sf::Vector3f position, sf::Vector3f forward, sf::Vector3f up, float focalLength, float focusStrength);

    void move(const sf::RenderWindow& window);
};

}