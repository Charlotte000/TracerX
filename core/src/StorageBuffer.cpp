/**
 * @file StorageBuffer.cpp
 */
#include "TracerX/StorageBuffer.h"

using namespace TracerX::core;

void StorageBuffer::init()
{
    glGenBuffers(1, &this->handler);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->handler);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void StorageBuffer::update(const void* data, size_t size)
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->handler);
    {
        if (this->size != size)
        {
            this->size = size;
            glBufferData(GL_SHADER_STORAGE_BUFFER, this->size, data, GL_STATIC_COPY);
        }
        else
        {
            glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, this->size, data);
        }
    }
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void StorageBuffer::bind(int binding)
{
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, this->handler);
}

void StorageBuffer::shutdown()
{
    glDeleteBuffers(1, &this->handler);
}
