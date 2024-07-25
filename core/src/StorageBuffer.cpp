/**
 * @file StorageBuffer.cpp
 */
#include "TracerX/BVH.h"
#include "TracerX/Mesh.h"
#include "TracerX/Vertex.h"
#include "TracerX/Triangle.h"
#include "TracerX/Material.h"
#include "TracerX/StorageBuffer.h"

using namespace TracerX;
using namespace TracerX::core;

void StorageBuffer::init()
{
    glGenBuffers(1, &this->handler);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->handler);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

template <class T>
void StorageBuffer::update(const std::vector<T>& data)
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->handler);

    size_t size = sizeof(T) * data.size();
    if (this->size != size)
    {
        this->size = size;
        glBufferData(GL_SHADER_STORAGE_BUFFER, this->size, data.data(), GL_STATIC_COPY);
    }
    else
    {
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, this->size, data.data());
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

template void StorageBuffer::update(const std::vector<Vertex>&);
template void StorageBuffer::update(const std::vector<Triangle>&);
template void StorageBuffer::update(const std::vector<Mesh>&);
template void StorageBuffer::update(const std::vector<Material>&);
template void StorageBuffer::update(const std::vector<Node>&);
