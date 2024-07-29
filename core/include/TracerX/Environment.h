/**
 * @file Environment.h
 */
#pragma once

#include "Texture.h"

#include <filesystem>
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
     * @brief The intensity of the environment.
     */
    float intensity = 1;

    /**
     * @brief The rotation matrix of the environment.
     */
    glm::mat3 rotation = glm::mat3(1);

    /**
     * @brief Indicates if the environment is transparent.
     */
    bool transparent = false;

    /**
     * @brief Loads environment data from a file.
     * @param path The path of the file to load the data from.
     */
    void loadFromFile(const std::filesystem::path& path);
private:
    core::Texture texture;

    friend class Renderer;
};

}
