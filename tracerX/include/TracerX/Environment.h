/**
 * @file Environment.h
 */
#pragma once

#include <OGL/Texture2D.h>

namespace TracerX
{

/**
 * @brief The environment settings for the Scene.
 * 
 * An environment is a 360-degree image that surrounds the scene.
 * It is used to provide background lighting and reflections.
 */
class Environment
{
public:
    /**
     * @brief The intensity of the environment.
     */
    float intensity = 1;

    /**
     * @brief The rotation matrix of the environment.
     */
    glm::mat3 rotation = glm::mat3(1);

    /**
     * @brief Indicates if the environment is transparent.
     * 
     * A transparent environment does contribute to the lighting of the scene but does not render as a background.
     */
    bool transparent = false;

    /**
     * @brief Loads environment image from an image.
     * @param image The image to load the environment from.
     */
    void update(const OGL::Image2D& image);
private:
    OGL::Texture2D texture = OGL::Texture2D(glm::uvec2(1), OGL::ImageFormat::RGBA32F);
    OGL::Texture2D cdfTexture = OGL::Texture2D(glm::uvec2(1), OGL::ImageFormat::R32F);
    float cdfTotal;

    void buildCDF(const OGL::Image2D& image);

    friend class Renderer;
};

}
