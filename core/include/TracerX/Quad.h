#pragma once

#include <GL/glew.h>


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
