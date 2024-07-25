/**
 * @file UniformBuffer.cpp
 */
#include "TracerX/BVH.h"
#include "TracerX/Mesh.h"
#include "TracerX/Vertex.h"
#include "TracerX/Triangle.h"
#include "TracerX/Material.h"
#include "TracerX/UniformBuffer.h"

#include <iostream>
using namespace TracerX;
using namespace TracerX::core;

void UniformBuffer::init()
{
    glGenBuffers(1, &this->handler);
    glBindBuffer(GL_UNIFORM_BUFFER, this->handler);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBuffer::update(const void* data, size_t size)
{
    glBindBuffer(GL_UNIFORM_BUFFER, this->handler);
    {
        if (this->size != size)
        {
            this->size = size;
            glBufferData(GL_UNIFORM_BUFFER, this->size, data, GL_STATIC_COPY);
        }
        else
        {
            glBufferSubData(GL_UNIFORM_BUFFER, 0, this->size, data);
        }
    }
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBuffer::updateSub(const void* data, size_t size, size_t offset)
{
    glBindBuffer(GL_UNIFORM_BUFFER, this->handler);
    {
        glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
    }
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBuffer::bind(int binding)
{
    glBindBufferBase(GL_UNIFORM_BUFFER, binding, this->handler);
}

void UniformBuffer::shutdown()
{
    glDeleteBuffers(1, &this->handler);
}