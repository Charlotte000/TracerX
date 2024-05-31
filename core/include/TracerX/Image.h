#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>

namespace TracerX::core
{

struct Image
{
public:
    glm::uvec2 size;
    std::vector<float> pixels;

    static Image empty;

    void saveToFile(const std::string& name) const;
    Image resize(glm::uvec2 size) const;

    static Image loadFromFile(const std::string& fileName);
    static Image loadFromMemory(glm::uvec2 size, const std::vector<float> pixels);
private:
    Image();
};

}
