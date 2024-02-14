#include "TracerX/Texture.h"


void Texture::init(int width, int height)
{
    this->width = width;
    this->height = height;

    glGenTextures(1, &this->handler);
    glBindTexture(GL_TEXTURE_2D, this->handler);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, this->width, this->height, 0, GL_RGB, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::init(const Image& image)
{
    this->width = image.width;
    this->height = image.height;

    glGenTextures(1, &this->handler);
    glBindTexture(GL_TEXTURE_2D, this->handler);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, this->width, this->height, 0, GL_RGB, GL_FLOAT, image.pixels.data());
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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, image.width, image.height, 0, GL_RGB, GL_FLOAT, image.pixels.data());

    this->width = image.width;
    this->height = image.height;
}

void Texture::update(int width, int height, float* pixels)
{
    glBindTexture(GL_TEXTURE_2D, this->handler);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, pixels);
    glBindTexture(GL_TEXTURE_2D, 0);

    this->width = width;
    this->height = height;
}

void Texture::upload(float* pixels)
{
    glBindTexture(GL_TEXTURE_2D, this->handler);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, pixels);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::upload(unsigned char* pixels)
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
