#pragma once

#include "Image.h"
#include "Texture.h"

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>

namespace TracerX::core
{

class TextureArray
{
public:
    glm::uvec3 size;

    void init();
    void bind(int binding);
    void update(glm::uvec2 size, const std::vector<Image>& images);
    void shutdown();
private:
    GLuint handler;
};

}
