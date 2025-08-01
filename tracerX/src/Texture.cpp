/**
 * @file Texture.cpp
 */
#include "TracerX/core/GL/Texture.h"

#include <GL/glew.h>

using namespace TracerX;
using namespace TracerX::core::GL;

void Texture::init(int internalFormat, int sampler)
{
    this->internalFormat = internalFormat;
    glGenTextures(1, &this->handler);
    glBindTexture(GL_TEXTURE_2D, this->handler);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, sampler);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, sampler);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::bindSampler(unsigned int binding)
{
    glBindTextureUnit(binding, this->handler);
}

void Texture::bindImage(unsigned int binding, unsigned int access)
{
    glBindImageTexture(binding, this->handler, 0, GL_FALSE, 0, access, this->internalFormat);
}

glm::uvec2 Texture::getSize() const
{
    return this->size;
}

void Texture::update(const Image& image, glm::uvec2 pos)
{
    glBindTexture(GL_TEXTURE_2D, this->handler);
    {
        glTexSubImage2D(GL_TEXTURE_2D, 0, pos.x, pos.y, image.size.x, image.size.y, GL_RGBA, GL_FLOAT, image.pixels.data());
    }
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::update(const Image& image)
{
    glBindTexture(GL_TEXTURE_2D, this->handler);
    {
        if (image.size != this->size)
        {
            this->size = image.size;
            glTexImage2D(GL_TEXTURE_2D, 0, this->internalFormat, this->size.x, this->size.y, 0, GL_RGBA, GL_FLOAT, image.pixels.data());
        }
        else
        {
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, this->size.x, this->size.y, GL_RGBA, GL_FLOAT, image.pixels.data());
        }
    }
    glBindTexture(GL_TEXTURE_2D, 0);
}

Image Texture::upload() const
{
    Image img(this->size);
    glBindTexture(GL_TEXTURE_2D, this->handler);
    {
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, img.pixels.data());
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    return img;
}

Image Texture::upload(glm::uvec2 pos, glm::uvec2 size) const
{
    Image img(size);
    glGetTextureSubImage(this->handler, 0, pos.x, pos.y, 0, size.x, size.y, 1, GL_RGBA, GL_FLOAT, img.pixels.size() * sizeof(float), img.pixels.data());
    return img;
}

void Texture::clear()
{
    glClearTexImage(this->handler, 0, GL_RGBA, GL_FLOAT, 0);
}

void Texture::resize(glm::uvec2 size)
{
    this->update(Image(size));
}

void Texture::shutdown()
{
    glDeleteTextures(1, &this->handler);
}

unsigned int Texture::getHandler() const
{
    return this->handler;
}
