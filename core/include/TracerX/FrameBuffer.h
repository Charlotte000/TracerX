#pragma once

#include "Texture.h"

#include <GL/glew.h>

namespace TracerX::core
{

class FrameBuffer
{
public:
    Texture colorTexture;

    void init();
    void shutdown();
    void use();
    void clear();

    static void stopUse(); 
private:
    GLuint handler;
};

}
