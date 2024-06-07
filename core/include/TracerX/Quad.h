/**
 * @file Quad.h
 */
#pragma once

#include <GL/glew.h>

namespace TracerX::core
{

struct Quad
{
public:
    void init();
    void draw();
    void shutdown();
private:
    GLuint handler;
    GLuint vertexHandler;
};

}
