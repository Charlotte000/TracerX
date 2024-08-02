/**
 * @file UniformBuffer.h
 */
#pragma once

#include <vector>
#include <GL/glew.h>

namespace TracerX::core
{

class UniformBuffer
{
public:
    void init();
    void update(const void* data, size_t size);
    void updateSub(const void* data, size_t size, size_t offset);
    void bind(unsigned int binding);
    void shutdown();
private:
    GLuint handler;
    size_t size;
};

}
