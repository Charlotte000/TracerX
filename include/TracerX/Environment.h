#pragma once

#include <SFML/Graphics.hpp>
#include "VectorMath.h"
#include "UBO.h"

namespace TracerX
{

struct Environment
{
    UBO<sf::Vector3f> skyColorHorizon;
    UBO<sf::Vector3f> skyColorZenith;
    UBO<sf::Vector3f> groundColor;
    UBO<sf::Vector3f> sunColor;
    UBO<sf::Vector3f> sunDirection;
    UBO<float> sunFocus;
    UBO<float> sunIntensity;
    UBO<float> skyIntensity;
    UBO<bool> enabled;

    void create(sf::Shader* shader);

    void updateShader();
};

}