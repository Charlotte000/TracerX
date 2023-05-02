#include <TracerX/Material.h>

namespace TracerX
{

Material::Material(sf::Vector3f albedoColor, float roughness, sf::Vector3f metalnessColor, float metalness, sf::Vector3f emissionColor, float emissionStrength, sf::Vector3f fresnelColor, float fresnelStrength, float refractionFactor)
    : albedoColor(albedoColor), roughness(roughness), emissionColor(emissionColor), emissionStrength(emissionStrength), metalnessColor(metalnessColor), metalness(metalness), fresnelColor(fresnelColor), fresnelStrength(fresnelStrength), refractionFactor(refractionFactor)
{
}

Material Material::LightSource(sf::Vector3f color, float strength)
{
    Material m;
    m.emissionColor = color;
    m.emissionStrength = strength;
    return m;
}

Material Material::Transparent(sf::Vector3f albedoColor, float refractionFactor, sf::Vector3f fresnelColor, float fresnelStrength)
{
    Material m(albedoColor, 1);
    m.refractionFactor = refractionFactor;
    m.fresnelColor = fresnelColor;
    m.fresnelStrength = fresnelStrength;
    return m;
}

}