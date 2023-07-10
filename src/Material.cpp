#include <TracerX/Material.h>

namespace TracerX
{

Material::Material(sf::Vector3f albedoColor, float roughness, sf::Vector3f metalnessColor, float metalness, sf::Vector3f emissionColor, float emissionStrength, sf::Vector3f fresnelColor, float fresnelStrength, float refractionFactor, float density)
    : albedoColor(albedoColor), roughness(roughness), emissionColor(emissionColor), emissionStrength(emissionStrength), metalnessColor(metalnessColor), metalness(metalness), fresnelColor(fresnelColor), fresnelStrength(fresnelStrength), refractionFactor(refractionFactor), density(density)
{
}

Material Material::LightSource(sf::Vector3f emissionColor, float emissionStrength)
{
    return Material(sf::Vector3f(0, 0, 0), 0, sf::Vector3f(1, 1, 1), 0, emissionColor, emissionStrength);
}

Material Material::Transparent(sf::Vector3f albedoColor, float refractionFactor, sf::Vector3f fresnelColor, float fresnelStrength)
{
    Material m(albedoColor);
    m.refractionFactor = refractionFactor;
    m.fresnelColor = fresnelColor;
    m.fresnelStrength = fresnelStrength;
    return m;
}

Material Material::ConstantDensity(sf::Vector3f albedoColor, float density)
{
    Material m(albedoColor);
    m.refractionFactor = 1.0f;
    m.density = density;
    return m;
}

Material Material::Matte(sf::Vector3f albedoColor, float metalness)
{
    return Material(albedoColor, 1, sf::Vector3f(1, 1, 1), metalness);
}

Material Material::Mirror()
{
    return Material(sf::Vector3f(1, 1, 1), 0, sf::Vector3f(1, 1, 1), 1);
}

bool operator==(const Material& a, const Material& b)
{
    return a.albedoColor == b.albedoColor &&
        a.albedoMapId == b.albedoMapId &&
        a.roughness == b.roughness &&
        a.emissionColor == b.emissionColor &&
        a.emissionStrength == b.emissionStrength &&
        a.metalnessColor == b.metalnessColor &&
        a.metalness == b.metalness &&
        a.fresnelColor == b.fresnelColor &&
        a.fresnelStrength == b.fresnelStrength &&
        a.refractionFactor == b.refractionFactor &&
        a.density == b.density;
}

}