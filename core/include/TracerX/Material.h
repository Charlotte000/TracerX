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
    float density;

    Material(sf::Vector3f albedoColor = sf::Vector3f(1, 1, 1), float roughness = 1, sf::Vector3f metalnessColor = sf::Vector3f(1, 1, 1), float metalness = 0, sf::Vector3f emissionColor = sf::Vector3f(1, 1, 1), float emissionStrength = 0, sf::Vector3f fresnelColor = sf::Vector3f(1, 1, 1), float fresnelStrength = 0, float refractionFactor = 0, float density = 0);

    static Material LightSource(sf::Vector3f emissionColor, float emissionStrength);

    static Material Transparent(sf::Vector3f albedoColor, float refractionFactor, sf::Vector3f fresnelColor, float fresnelStrength);

    static Material ConstantDensity(sf::Vector3f albedoColor, float density);

    static Material Matte(sf::Vector3f albedoColor, float metalness = 0);

    static Material Mirror();

    friend bool operator==(const Material& a, const Material& b);
};

}