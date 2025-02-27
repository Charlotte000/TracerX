/**
 * @file Texture.h
 */
#pragma once

#include "TracerX/Image.h"

namespace TracerX::core::GL
{

/**
 * @brief An OpenGL 2D texture wrapper class.
 * @remark Should not be used directly and is only used internally by the renderer.
 */
class Texture
{
public:
    glm::uvec2 size;

    void init(int internalFormat, int sampler);
    void bind(unsigned int binding);
    void bindImage(unsigned int binding, unsigned int access);
    void update(const Image& image);
    Image upload() const;
    void clear();
    void resize(glm::uvec2 size);
    void shutdown();
    unsigned int getHandler() const;
private:
    unsigned int handler;
    int internalFormat;
};

}
