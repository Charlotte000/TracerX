/**
 * @file Texture.cpp
 */
#include "TracerX/Texture.h"

using namespace TracerX;
using namespace TracerX::core;

void Texture::init(GLint internalFormat)
{
    this->internalFormat = internalFormat;
    glGenTextures(1, &this->handler);
    glBindTexture(GL_TEXTURE_2D, this->handler);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::bind(unsigned int binding)
{
    glBindTextureUnit(binding, this->handler);
}

void Texture::bindImage(unsigned int binding, GLenum access)
{
    glBindImageTexture(binding, this->handler, 0, GL_FALSE, 0, access, this->internalFormat);
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
    std::vector<float> pixels(this->size.x * this->size.y * 4);
    glBindTexture(GL_TEXTURE_2D, this->handler);
    {
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, pixels.data());
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    return Image::loadFromMemory(this->size, pixels);
}

void Texture::clear()
{
    glClearTexImage(this->handler, 0, GL_RGBA, GL_FLOAT, 0);
}

void Texture::resize(glm::uvec2 size)
{
    this->update(Image::loadFromMemory(size, std::vector<float>(size.x * size.y * 4, 0)));
}

void Texture::shutdown()
{
    glDeleteTextures(1, &this->handler);
}

GLuint Texture::getHandler() const
{
    return this->handler;
}
