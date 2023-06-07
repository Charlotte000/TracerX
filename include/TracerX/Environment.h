#pragma once

#include <SFML/Graphics.hpp>
#include <TracerX/VectorMath.h>

namespace TracerX
{

struct Environment
{
    sf::Vector3f skyColorHorizon = sf::Vector3f(1, 1, 1);
    sf::Vector3f skyColorZenith = sf::Vector3f(.5f, .5f, 1);
    sf::Vector3f groundColor = sf::Vector3f(.5f, .5f, .5f);
    sf::Vector3f sunColor = sf::Vector3f(1, 1, 1);
    sf::Vector3f sunDirection = normalized(sf::Vector3f(0, -1, -1));
    float sunFocus = 800;
    float sunIntensity = 10;
    float skyIntensity = .01f;
    bool enabled = true;

    Environment();

    Environment(sf::Vector3f skyColorHorizon, sf::Vector3f skyColorZenith, sf::Vector3f groundColor, sf::Vector3f sunColor, sf::Vector3f sunDirection, float sunFocus, float sunIntensity, float skyIntensity);

    void set(sf::Shader& shader, const std::string name);
};

}