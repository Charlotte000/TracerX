#include "TracerX/Texture.h"


void Texture::init(glm::ivec2 size)
{
    this->size = size;

    glGenTextures(1, &this->handler);
    glBindTexture(GL_TEXTURE_2D, this->handler);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, this->size.x, this->size.y, 0, GL_RGB, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::init(const Image& image)
{
    this->size = image.size;

    glGenTextures(1, &this->handler);
    glBindTexture(GL_TEXTURE_2D, this->handler);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, this->size.x, this->size.y, 0, GL_RGB, GL_FLOAT, image.pixels.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::bind(int binding)
{
    glActiveTexture(GL_TEXTURE0 + binding);
    glBindTexture(GL_TEXTURE_2D, this->handler);
}

void Texture::update(const Image& image)
{
    glBindTexture(GL_TEXTURE_2D, this->handler);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, image.size.x, image.size.y, 0, GL_RGB, GL_FLOAT, image.pixels.data());
    glBindTexture(GL_TEXTURE_2D, 0);

    this->size = image.size;
}

void Texture::update(glm::ivec2 size, float* pixels)
{
    glBindTexture(GL_TEXTURE_2D, this->handler);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, size.x, size.y, 0, GL_RGB, GL_FLOAT, pixels);
    glBindTexture(GL_TEXTURE_2D, 0);

    this->size = size;
}

void Texture::upload(float* pixels) const
{
    glBindTexture(GL_TEXTURE_2D, this->handler);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, pixels);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::upload(unsigned char* pixels) const
{
    glBindTexture(GL_TEXTURE_2D, this->handler);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::shutdown()
{
    glDeleteTextures(1, &this->handler);
}

GLuint Texture::getHandler() const
{
    return this->handler;
}
