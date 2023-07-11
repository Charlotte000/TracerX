#pragma once

#include <SFML/Graphics.hpp>
#include <TracerX/UBO.h>

namespace TracerX
{

struct Camera
{
    UBO<sf::Vector3f> position;
    UBO<sf::Vector3f> forward;
    UBO<sf::Vector3f> up;
    UBO<sf::Vector3f> right;
    UBO<float> focalLength;
    UBO<float> focusStrength;
    UBO<float> fov;

    Camera();

    Camera(sf::Vector3f position, sf::Vector3f forward, sf::Vector3f up, float focalLength, float focusStrength, float fov = 3.1415f / 2);

    void create(sf::Shader* shader);

    void updateShader();

    void move(const sf::RenderWindow& window);

    void lookAt(sf::Vector3f position);
};

}