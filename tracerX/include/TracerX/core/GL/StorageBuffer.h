/**
 * @file StorageBuffer.h
 */
#pragma once

#include <cstddef>

namespace TracerX::core::GL
{

/**
 * @brief An OpenGL storage buffer object (SSBO) wrapper class.
 * 
 * The storage buffer is used to store data that is shared between the CPU and the GPU, such as vertex, triangle, and material data.
 * 
 * @remark Should not be used directly and is only used internally by the renderer.
 */
class StorageBuffer
{
public:
    void init();
    void update(const void* data, size_t size);
    void bindBuffer(unsigned int binding);
    void shutdown();
private:
    unsigned int handler;
    size_t size;
};

}
