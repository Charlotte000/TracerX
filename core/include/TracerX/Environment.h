/**
 * @file Environment.h
 */
#pragma once

#include "TracerX/core/GL/Texture.h"

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
     * @brief Loads environment image from a file.
     * @param path The path of the file to load the image from.
     */
    void loadFromFile(const std::filesystem::path& path);

    /**
     * @brief Loads environment image from an image.
     * @param image The image to load the environment from.
     */
    void loadFromImage(const Image& image);
private:
    core::GL::Texture texture;

    friend class Renderer;
};

}
