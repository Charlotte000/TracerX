/**
 * @file Material.h
 */
#pragma once

#include <glm/glm.hpp>

namespace TracerX
{

/**
 * @brief Describes the properties of the surface of an object and how it interacts with light according to Disney's BSDF model.
 */
struct Material
{
    /**
     * @brief The color of the material's albedo.
     * 
     * The albedo is the base color of the material.
     */
    glm::vec3 albedoColor = glm::vec3(1);

    /**
     * @brief The roughness of the material's surface.
     * 
     * The roughness is a value between 0 and 1, where 0 is a perfectly smooth surface and 1 is a perfectly rough surface.
     */
    float roughness = 0;

    /**
     * @brief The color of the material's emission.
     * 
     * The emission is the light emitted by the material.
     */
    glm::vec3 emissionColor = glm::vec3(1);

    /**
     * @brief The strength of the material's emission.
     */
    float emissionStrength = 0;

    /**
     * @brief The color of the material's fresnel effect.
     * 
     * The fresnel effect is the reflection of light off of a surface at a shallow angle.
     */
    glm::vec3 fresnelColor = glm::vec3(1);

    /**
     * @brief The strength of the material's fresnel effect.
     * 
     * The fresnel effect is the reflection of light off of a surface at a shallow angle.
     */
    float fresnelStrength = 0;

    /**
     * @brief The metalness of the material.
     * 
     * The metalness is a value between 0 and 1, where 0 is a dielectric material and 1 is a metal.
     */
    float metalness = 0;

    /**
     * @brief The index of refraction of the material.
     * 
     * The index of refraction is a value that describes how light bends as it passes through the material.
     * If the index of refraction is 0, the material is opaque.
     */
    float ior = 0;

    /**
     * @brief The density of the material.
     * 
     * The density is a value that describes how light is absorbed as it passes through the material.
     * If the density is 0, the material is opaque.
     */
    float density = 0;

    /**
     * @brief The texture ID for the material's albedo.
     * 
     * The texture ID is the index of the texture in the renderer's texture array.
     * If the texture ID is -1, the material uses the Material::albedoColor value. Otherwise, the material multiplies the albedo color by the texture color.
     */
    int albedoTextureId = -1;

    /**
     * @brief The texture ID for the material's metalness.
     * 
     * The texture ID is the index of the texture in the renderer's texture array.
     * If the texture ID is -1, the material uses the Material::metalness value. Otherwise, the material multiplies the metalness value by the texture color.
     */
    int metalnessTextureId = -1;

    /**
     * @brief The texture ID for the material's emission.
     * 
     * The texture ID is the index of the texture in the renderer's texture array.
     * If the texture ID is -1, the material uses the Material::emissionColor value. Otherwise, the material multiplies the emission color by the texture color.
     */
    int emissionTextureId = -1;

    /**
     * @brief The texture ID for the material's roughness.
     * 
     * The texture ID is the index of the texture in the renderer's texture array.
     * If the texture ID is -1, the material uses the Material::roughness value. Otherwise, the material multiplies the roughness value by the texture color.
     */
    int roughnessTextureId = -1;

    /**
     * @brief The texture ID for the material's normal map.
     * 
     * The texture ID is the index of the texture in the renderer's texture array.
     * If the texture ID is -1, the material does not use a normal map. Otherwise, the material uses the texture as a normal map.
     */
    int normalTextureId = -1;

    /**
     * @brief Determines how the material uses the alpha value.
     */
    enum class AlphaMode : unsigned int
    {
        /**
         * @brief The alpha value is ignored.
         */
        Opaque = 0,

        /**
         * @brief The alpha value is used to blend the material with the background.
         */
        Blend = 1,

        /**
         * @brief The alpha value is used to determine if a pixel is transparent or opaque using Material::alphaCutoff.
         */
        Mask = 2,
    }
    /**
     * @brief The alpha mode used by the material.
     */
    alphaMode = AlphaMode::Opaque;

    /**
     * @brief The alpha cutoff value for the material.
     * 
     * Only used when the alpha mode is set to AlphaMode::Mask.
     * The alpha cutoff value is a threshold value that determines if a pixel is transparent or opaque.
     * 
     * @see Material::alphaMode
     */
    float alphaCutoff = .5f;

    /**
     * @brief Creates a light source material with the given emission color and strength.
     * @param emissionColor The color of the emission.
     * @param emissionStrength The strength of the emission.
     * @return The light source material.
     */
    static Material lightSource(glm::vec3 emissionColor, float emissionStrength);

    /**
     * @brief Creates a transparent material with the given albedo color, index of refraction, fresnel color, and fresnel strength.
     * @param albedoColor The color of the albedo.
     * @param ior The index of refraction.
     * @param fresnelColor The color of the fresnel effect.
     * @param fresnelStrength The strength of the fresnel effect.
     * @return The transparent material.
     */
    static Material transparent(glm::vec3 albedoColor, float ior, glm::vec3 fresnelColor = glm::vec3(1), float fresnelStrength = 0);

    /**
     * @brief Creates a material with constant density with the given albedo color and density.
     * @param albedoColor The color of the albedo.
     * @param density The density.
     * @return The material with constant density.
     */
    static Material constantDensity(glm::vec3 albedoColor, float density);

    /**
     * @brief Creates a matte material with the given albedo color and metalness.
     * @param albedoColor The color of the albedo.
     * @param metalness The metalness.
     * @return The matte material.
     */
    static Material matte(glm::vec3 albedoColor, float metalness = 0);

    /**
     * @brief Creates a mirror material.
     * @return The mirror material.
     */
    static Material mirror();
private:
    int padding1;
    int padding2;
};

}
