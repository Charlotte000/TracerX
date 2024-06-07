/**
 * @file Environment.h
 */
#pragma once

#include "Texture.h"

#include <string>
#include <glm/glm.hpp>

namespace TracerX
{

/**
 * @brief Represents an environment in the TracerX system.
 */
class Environment
{
public:
    /**
     * @brief The name of the environment.
     */
    std::string name = "None";

    /**
     * @brief The intensity of the environment.
     */
    float intensity = 1.f;

    /**
     * @brief The rotation matrix of the environment.
     */
    glm::mat3 rotation = glm::mat3(1);

    /**
     * @brief Indicates if the environment is transparent.
     */
    bool transparent = false;

    /**
     * @brief Resets the environment to its default state.
     * 
     * This method sets the name of the environment to "None" and updates the texture with an empty black image.
     */
    void reset();

    /**
     * @brief Loads environment data from a file.
     * @param fileName The name of the file to load the data from.
     */
    void loadFromFile(const std::string& fileName);
private:
    core::Texture texture;

    friend class Renderer;
};

}
