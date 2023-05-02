#pragma once

#include <SFML/Graphics.hpp>
#include <TracerX/VectorMath.h>

namespace TracerX
{

struct Environment
{
    sf::Vector3f skyColorHorizon = sf::Vector3f(.01f, .01f, .01f);
    sf::Vector3f skyColorZenith = sf::Vector3f(.005f, .005f, .01f);
    sf::Vector3f groundColor = sf::Vector3f(.005f, .005f, .005f);
    sf::Vector3f sunColor = sf::Vector3f(1, 1, 1);
    sf::Vector3f sunDirection = normalized(sf::Vector3f(0, -1, -1));
    float sunFocus = 800;
    float sunIntensity = 10;
    bool enabled = true;

    Environment();

    Environment(sf::Vector3f skyColorHorizon, sf::Vector3f skyColorZenith, sf::Vector3f groundColor, sf::Vector3f sunColor, sf::Vector3f sunDirection, float sunFocus, float sunIntensity);

    void set(sf::Shader& shader, const std::string name);
};

}