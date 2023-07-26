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
    UBO<float> focalLength;
    UBO<float> focusStrength;
    UBO<float> fov;

    Camera();

    Camera(sf::Vector3f position, sf::Vector3f forward, sf::Vector3f up, float focalLength, float focusStrength, float fov = 3.1415f / 2);

    void create(sf::Shader* shader, const std::string& name);

    void updateShader();

    void lookAt(sf::Vector3f position);

    sf::Vector3f getRight();

    void set(const Camera& newCamera);
private:
    std::string name;
};

}