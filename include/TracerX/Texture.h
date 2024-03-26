#pragma once

#include "Image.h"

#include <string>
#include <GL/glew.h>


class Texture
{
public:
    glm::ivec2 size;

    void init();
    void bind(int binding);
    void update(const Image& image);
    Image upload(const std::string& name) const;
    void shutdown();
    GLuint getHandler() const;
private:
    GLuint handler;
};
