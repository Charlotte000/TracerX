/**
 * @file UniformBuffer.h
 */
#pragma once

#include <vector>
#include <GL/glew.h>

namespace TracerX::core
{

/**
 * @brief An OpenGL uniform buffer object (UBO) wrapper class.
 * 
 * The uniform buffer is used to store data that is shared between the CPU and the GPU, such as camera and environment settings.
 * 
 * @remark Should not be used directly and is only used internally by the renderer.
 */
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
