/**
 * @file Material.h
 */
#pragma once

#include <glm/glm.hpp>

namespace TracerX::core
{

/**
 * @brief Represents the properties of a material used in rendering.
 */
struct Material
{
    /**
     * @brief The color of the material's albedo.
     */
    glm::vec3 albedoColor = glm::vec3(1);

    /**
     * @brief The roughness of the material's surface.
     */
    float roughness = 0;

    /**
     * @brief The color of the material's emission.
     */
    glm::vec3 emissionColor = glm::vec3(1);

    /**
     * @brief The strength of the material's emission.
     */
    float emissionStrength = 0;

    /**
     * @brief The color of the material's fresnel effect.
     */
    glm::vec3 fresnelColor = glm::vec3(1);

    /**
     * @brief The strength of the material's fresnel effect.
     */
    float fresnelStrength = 0;

    /**
     * @brief The metalness of the material.
     */
    float metalness = 0;

    /**
     * @brief The index of refraction of the material.
     */
    float ior = 0;

    /**
     * @brief The density of the material.
     */
    float density = 0;

    /**
     * @brief The texture ID for the material's albedo.
     */
    float albedoTextureId = -1;

    /**
     * @brief The texture ID for the material's metalness.
     */
    float metalnessTextureId = -1;

    /**
     * @brief The texture ID for the material's emission.
     */
    float emissionTextureId = -1;

    /**
     * @brief The texture ID for the material's roughness.
     */
    float roughnessTextureId = -1;
    
    /**
     * @brief The texture ID for the material's normal map.
     */
    float normalTextureId = -1;

    /**
     * @brief Creates a light source material with the given emission color and strength.
     * 
     * @param emissionColor The color of the emission.
     * @param emissionStrength The strength of the emission.
     * @return The light source material.
     */
    static Material lightSource(glm::vec3 emissionColor, float emissionStrength);

    /**
     * @brief Creates a transparent material with the given albedo color, index of refraction, fresnel color, and fresnel strength.
     * 
     * @param albedoColor The color of the albedo.
     * @param ior The index of refraction.
     * @param fresnelColor The color of the fresnel effect.
     * @param fresnelStrength The strength of the fresnel effect.
     * @return The transparent material.
     */
    static Material transparent(glm::vec3 albedoColor, float ior, glm::vec3 fresnelColor = glm::vec3(1), float fresnelStrength = 0);

    /**
     * @brief Creates a material with constant density with the given albedo color and density.
     * 
     * @param albedoColor The color of the albedo.
     * @param density The density.
     * @return The material with constant density.
     */
    static Material constantDensity(glm::vec3 albedoColor, float density);

    /**
     * @brief Creates a matte material with the given albedo color and metalness.
     * 
     * @param albedoColor The color of the albedo.
     * @param metalness The metalness.
     * @return The matte material.
     */
    static Material matte(glm::vec3 albedoColor, float metalness = 0);

    /**
     * @brief Creates a mirror material.
     * 
     * @return The mirror material.
     */
    static Material mirror();
};

}
