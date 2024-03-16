#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>


struct Image
{
    glm::ivec2 size;
    std::vector<float> pixels;
    std::string name = "None";

    static Image empty;

    Image();

    static Image loadFromFile(const std::string& fileName);
    static void saveToDisk(const std::string& fileName, glm::ivec2 size, unsigned char* pixels);
};
