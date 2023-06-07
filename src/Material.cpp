#include <TracerX/Material.h>

namespace TracerX
{

Material::Material(sf::Vector3f albedoColor, float roughness, sf::Vector3f metalnessColor, float metalness, sf::Vector3f emissionColor, float emissionStrength, sf::Vector3f fresnelColor, float fresnelStrength, float refractionFactor, float density)
    : albedoColor(albedoColor), roughness(roughness), emissionColor(emissionColor), emissionStrength(emissionStrength), metalnessColor(metalnessColor), metalness(metalness), fresnelColor(fresnelColor), fresnelStrength(fresnelStrength), refractionFactor(refractionFactor), density(density)
{
}

Material Material::LightSource(sf::Vector3f color, float strength)
{
    Material m;
    m.roughness = 0;
    m.emissionColor = color;
    m.emissionStrength = strength;
    return m;
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
    m.density = density;
    return m;
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