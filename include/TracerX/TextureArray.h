#pragma once

#include "Image.h"
#include "Texture.h"

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>


class TextureArray
{
public:
    glm::ivec2 size;

    void init();
    void bind(int binding);
    void update(glm::ivec2 size, const std::vector<Image>& images);
    void copy(Texture& texture, size_t index) const;
    void shutdown();
private:
    GLuint handler;
};
