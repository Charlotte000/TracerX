/**
 * @file FrameBuffer.cpp
 */
#include "TracerX/Image.h"
#include "TracerX/FrameBuffer.h"

using namespace TracerX::core;

void FrameBuffer::init()
{
    // Create framebuffer
    glGenFramebuffers(1, &this->handler);
    glBindFramebuffer(GL_FRAMEBUFFER, this->handler);

    // Attach accumulation texture
    this->accumulation.init();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->accumulation.getHandler(), 0);

    // Attach albedo texture
    this->albedo.init();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, this->albedo.getHandler(), 0);

    // Attach normal texture
    this->normal.init();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, this->normal.getHandler(), 0);

    // Attach tone map texture
    this->toneMap.init();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, this->toneMap.getHandler(), 0);

    GLenum attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
    glDrawBuffers(4, attachments);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::resize(glm::uvec2 size)
{
    this->size = size;
    this->accumulation.update(Image::loadFromMemory(size, std::vector<float>()));
    this->albedo.update(Image::loadFromMemory(size, std::vector<float>()));
    this->normal.update(Image::loadFromMemory(size, std::vector<float>()));
    this->toneMap.update(Image::loadFromMemory(size, std::vector<float>()));
}

void FrameBuffer::shutdown()
{
    this->accumulation.shutdown();
    this->albedo.shutdown();
    this->normal.shutdown();
    this->toneMap.shutdown();
    glDeleteFramebuffers(1, &this->handler);
}

void FrameBuffer::use()
{
    glBindFramebuffer(GL_FRAMEBUFFER, this->handler);
    glViewport(0, 0, this->size.x, this->size.y);
}

void FrameBuffer::useRect(glm::uvec2 position, glm::uvec2 size)
{
    glBindFramebuffer(GL_FRAMEBUFFER, this->handler);
    glViewport(0, 0, this->size.x, this->size.y);
    glEnable(GL_SCISSOR_TEST);
    glScissor(position.x, position.y, size.x, size.y);
}

void FrameBuffer::clear()
{
    glClearTexImage(this->accumulation.getHandler(), 0, GL_RGBA, GL_FLOAT, 0);
}

void FrameBuffer::stopUse()
{
    glDisable(GL_SCISSOR_TEST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
