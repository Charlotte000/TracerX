#pragma once

#include <SFML/Graphics.hpp>

namespace TracerX
{

struct Material
{
    alignas(16) sf::Vector3f albedoColor;
    alignas(16) sf::Vector3f metalnessColor;
    alignas(16) sf::Vector3f emissionColor;
    alignas(16) sf::Vector3f fresnelColor;
    int albedoMapId = -1;
    float roughness;
    float metalness;
    float emissionStrength;
    float fresnelStrength;
    float refractionFactor;

    Material(sf::Vector3f albedoColor = sf::Vector3f(0, 0, 0), float roughness = 1, sf::Vector3f metalnessColor = sf::Vector3f(0, 0, 0), float metalness = 0, sf::Vector3f emissionColor = sf::Vector3f(0, 0, 0), float emissionStrength = 0, sf::Vector3f fresnelColor = sf::Vector3f(0, 0, 0), float fresnelStrength = 0, float refractionFactor = 0);

    static Material LightSource(sf::Vector3f color, float strength);

    static Material Transparent(sf::Vector3f albedoColor, float refractionFactor, sf::Vector3f fresnelColor, float fresnelStrength);

    friend bool operator==(const Material& a, const Material& b);
};

}