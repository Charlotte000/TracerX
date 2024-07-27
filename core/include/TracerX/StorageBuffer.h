/**
 * @file StorageBuffer.h
 */
#pragma once

#include <GL/glew.h>

namespace TracerX::core
{

class StorageBuffer
{
public:
    void init();
    void update(const void* data, size_t size);
    void bind(int binding);
    void shutdown();
private:
    GLuint handler;
    size_t size;
};

}
