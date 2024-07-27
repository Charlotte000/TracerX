/**
 * @file Material.cpp
 */
#include "TracerX/Material.h"

using namespace TracerX;

Material Material::lightSource(glm::vec3 emissionColor, float emissionStrength)
{
    Material m;
    m.albedoColor = glm::vec3(0);
    m.emissionColor = emissionColor * emissionStrength;
    return m;
}

Material Material::transparent(glm::vec3 albedoColor, float ior, glm::vec3 fresnelColor, float fresnelStrength)
{
    Material m;
    m.albedoColor = albedoColor;
    m.ior = ior;
    m.fresnelColor = fresnelColor;
    m.fresnelStrength = fresnelStrength;
    return m;
}

Material Material::constantDensity(glm::vec3 albedoColor, float density)
{
    Material m;
    m.roughness = 1;
    m.albedoColor = albedoColor;
    m.ior = 1;
    m.density = density;
    return m;
}

Material Material::matte(glm::vec3 albedoColor, float metalness)
{
    Material m;
    m.albedoColor = albedoColor;
    m.roughness = 1;
    m.metalness = metalness;
    return m;
}

Material Material::mirror()
{
    Material m;
    m.roughness = 0;
    m.metalness = 1;
    return m;
}
