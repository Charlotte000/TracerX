#pragma once

#include <GL/glew.h>


struct Quad
{
public:
    static void draw();
private:
    static GLuint handler;
    static bool initialized;

    static void init();
};
