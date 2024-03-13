#pragma once

#include "Image.h"

#include <GL/glew.h>


class Texture
{
public:
    glm::ivec2 size;

    void init(glm::ivec2 size);
    void init(const Image& image);
    void bind(int binding);
    void update(const Image& image);
    void update(glm::ivec2 size, float* pixels);
    void upload(float* pixels);
    void upload(unsigned char* pixels);
    void shutdown();
    GLuint getHandler() const;
private:
    GLuint handler;
};
