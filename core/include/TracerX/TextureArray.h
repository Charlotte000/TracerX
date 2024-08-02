/**
 * @file TextureArray.h
 */
#pragma once

#include "Image.h"

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>

namespace TracerX::core
{

class TextureArray
{
public:
    glm::uvec3 size;

    void init(GLint internalFormat);
    void bind(unsigned int binding);
    void update(const std::vector<Image>& images);
    void shutdown();
private:
    GLuint handler;
    GLint internalFormat;

    static glm::uvec2 getMaxSize(const std::vector<Image>& images);
    static std::vector<float> resizeImages(const std::vector<Image>& images, glm::uvec2 size);
};

}
