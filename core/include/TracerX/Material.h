#pragma once

#include <glm/glm.hpp>


struct Material
{
    glm::vec3 albedoColor = glm::vec3(1);
    float roughness = 0;
    glm::vec3 emissionColor = glm::vec3(1);
    float emissionStrength = 0;
    glm::vec3 fresnelColor = glm::vec3(1);
    float fresnelStrength = 0;
    float metalness = 0;
    float ior = 0;
    float density = 0;
    float albedoTextureId = -1;
    float metalnessTextureId = -1;
    float emissionTextureId = -1;
    float roughnessTextureId = -1;
    float normalTextureId = -1;

    static Material lightSource(glm::vec3 emissionColor, float emissionStrength);
    static Material transparent(glm::vec3 albedoColor, float ior, glm::vec3 fresnelColor = glm::vec3(1), float fresnelStrength = 0);
    static Material constantDensity(glm::vec3 albedoColor, float density);
    static Material matte(glm::vec3 albedoColor, float metalness = 0);
    static Material mirror();
};
