#pragma once

#include "Image.h"

#include <vector>
#include <GL/glew.h>


class TextureArray
{
public:
    glm::ivec2 size;

    void init(glm::ivec2 size, const std::vector<Image>& images);
    void bind(int binding);
    void update(glm::ivec2 size, const std::vector<Image>& images);
    void shutdown();
private:
    GLuint handler;
};