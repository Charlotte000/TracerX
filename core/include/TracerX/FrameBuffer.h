/**
 * @file FrameBuffer.h
 */
#pragma once

#include "Texture.h"

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>

namespace TracerX::core
{

class FrameBuffer
{
public:
    glm::uvec2 size;
    std::vector<Texture> textures;

    void init(const std::vector<GLenum>& attachments);
    void resize(glm::uvec2 size);
    void shutdown();
    void use();
    void useRect(glm::uvec2 position, glm::uvec2 size);
    void clear();

    static void stopUse(); 
private:
    GLuint handler;
    std::vector<GLenum> attachments;
};

}
