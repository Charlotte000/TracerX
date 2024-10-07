/**
 * @file Texture.h
 */
#pragma once

#include "Image.h"

#include <GL/glew.h>
#include <glm/glm.hpp>

namespace TracerX::core
{

/**
 * @brief An OpenGL 2D texture wrapper class.
 * @remark Should not be used directly and is only used internally by the renderer.
 */
class Texture
{
public:
    glm::uvec2 size;

    void init(GLint internalFormat, GLint sampler = GL_LINEAR);
    void bind(unsigned int binding);
    void bindImage(unsigned int binding, GLenum access = GL_READ_WRITE);
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
