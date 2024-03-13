#pragma once

#include "Texture.h"

#include <GL/glew.h>


class FrameBuffer
{
public:
    Texture colorTexture;

    void init(glm::ivec2 size);
    void shutdown();
    void draw();
    void clear();
private:
    GLuint handler;
};
