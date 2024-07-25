/**
 * @file StorageBuffer.h
 */
#pragma once

#include <vector>
#include <GL/glew.h>

namespace TracerX::core
{

class StorageBuffer
{
public:
    void init();

    template <class T>
    void update(const std::vector<T>& data);

    void bind(int binding);
    void shutdown();
private:
    GLuint handler;
    size_t size;
};

}
