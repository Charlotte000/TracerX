#pragma once

#include "Texture.h"

#include <GL/glew.h>


class FrameBuffer
{
public:
    Texture colorTexture;

    void init();
    void shutdown();
    void draw();
    void clear();
private:
    GLuint handler;
};
