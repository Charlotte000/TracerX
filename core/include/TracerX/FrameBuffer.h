/**
 * @file FrameBuffer.h
 */
#pragma once

#include "Texture.h"

#include <GL/glew.h>
#include <glm/glm.hpp>

namespace TracerX::core
{

class FrameBuffer
{
public:
    Texture colorTexture;

    void init();
    void shutdown();
    void use();
    void useRect(glm::uvec2 position, glm::uvec2 size);
    void clear();

    static void stopUse(); 
private:
    GLuint handler;
};

}
