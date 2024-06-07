/**
 * @file Buffer.h
 */
#pragma once

#include <vector>
#include <GL/glew.h>

namespace TracerX::core
{

template <class T>
class Buffer
{
public:
    void init(GLenum internalFormat);
    void update(const std::vector<T>& data);
    void bind(int binding);
    void shutdown();
private:
    GLuint handler;
    GLuint textureHandler;
    size_t size;
};

template <class T>
void Buffer<T>::init(GLenum internalFormat)
{
    glPixelStorei(GL_PACK_ALIGNMENT, 1);

    glGenBuffers(1, &this->handler);
    glBindBuffer(GL_TEXTURE_BUFFER, this->handler);
    glGenTextures(1, &this->textureHandler);
    glBindTexture(GL_TEXTURE_BUFFER, this->textureHandler);
    glTexBuffer(GL_TEXTURE_BUFFER, internalFormat, this->handler);

    glBindTexture(GL_TEXTURE_BUFFER, 0);
    glBindBuffer(GL_TEXTURE_BUFFER, 0);
}

template <class T>
void Buffer<T>::update(const std::vector<T>& data)
{
    glBindBuffer(GL_TEXTURE_BUFFER, this->handler);

    size_t size = sizeof(T) * data.size();
    if (this->size != size)
    {
        this->size = size;
        glBufferData(GL_TEXTURE_BUFFER, this->size, data.data(), GL_STATIC_DRAW);
    }
    else
    {
        glBufferSubData(GL_TEXTURE_BUFFER, 0, this->size, data.data());
    }

    glBindBuffer(GL_TEXTURE_BUFFER, 0);
}

template <class T>
void Buffer<T>::bind(int binding)
{
    glActiveTexture(GL_TEXTURE0 + binding);
    glBindTexture(GL_TEXTURE_BUFFER, this->textureHandler);
}

template <class T>
void Buffer<T>::shutdown()
{
    glDeleteTextures(1, &this->textureHandler);
    glDeleteBuffers(1, &this->handler);
}

}
