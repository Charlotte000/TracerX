/**
 * @file Texture.h
 */
#pragma once

#include "Image.h"

#include <GL/glew.h>

namespace TracerX::core
{

class Texture
{
public:
    glm::uvec2 size;

    void init(GLint internalFormat);
    void bind(int binding);
    void bindImage(int binding, GLenum access = GL_READ_WRITE);
    void update(const Image& image);
    Image upload() const;
    void clear();
    void resize(glm::uvec2 size);
    void shutdown();
    GLuint getHandler() const;
private:
    GLuint handler;
    GLint internalFormat;
};

}
