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

}
