#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>


struct Image
{
public:
    glm::ivec2 size;
    std::vector<float> pixels;
    std::string name = "None";

    static Image empty;

    void saveToFile() const;
    Image resize(glm::ivec2 size) const;

    static Image loadFromFile(const std::string& fileName);
    static Image loadFromMemory(const std::string& name, glm::ivec2 size, const std::vector<float> pixels);
private:
    Image();
};
