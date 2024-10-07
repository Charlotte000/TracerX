/**
 * @file Image.h
 */
#pragma once

#include <vector>
#include <filesystem>
#include <glm/glm.hpp>

namespace TracerX
{

/**
 * @brief An array of pixel data stored in RAM.
 */
struct Image
{
public:
    /**
     * @brief The size of the image.
     */
    glm::uvec2 size;

    /**
     * @brief The pixel data of the image.
     * 
     * The pixel data is stored in the order red, green, blue, alpha.
     * The size of the array is size.x * size.y * 4.
     */
    std::vector<float> pixels;

    /**
     * @brief An empty image.
     */
    static Image empty;

    /**
     * @brief Saves the image to a PNG file.
     * @param path The path of the file to save the image to.
     */
    void saveToFile(const std::filesystem::path& path) const;
 
    /**
     * @brief Resizes the image to the specified size.
     * @param size The new size of the image.
     * @return The resized image.
     */
    Image resize(glm::uvec2 size) const;

    /**
     * @brief Loads an image from a file.
     * @param path The path of the file to load the image from.
     * @return The loaded image.
     * @throws std::runtime_error Thrown if the image fails to load.
     */
    static Image loadFromFile(const std::filesystem::path& path);

    /**
     * @brief Loads an image from memory.
     * @param size The size of the image.
     * @param pixels The pixel data of the image.
     * @remark The pixel data is stored in the order red, green, blue, alpha.
     * @remark The size of the array must be size.x * size.y * 4.
     * @return The loaded image.
     */
    static Image loadFromMemory(glm::uvec2 size, const std::vector<float> pixels);
private:
    Image();
};

}
