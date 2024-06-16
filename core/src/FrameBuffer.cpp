/**
 * @file FrameBuffer.cpp
 */
#include "TracerX/Image.h"
#include "TracerX/FrameBuffer.h"

using namespace TracerX::core;

void FrameBuffer::init(const std::vector<GLenum>& attachments)
{
    this->attachments = attachments;

    // Create framebuffer
    glGenFramebuffers(1, &this->handler);
    glBindFramebuffer(GL_FRAMEBUFFER, this->handler);

    // Attach textures
    for (const GLenum attachment : this->attachments)
    {
        Texture texture;
        texture.init();
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture.getHandler(), 0);
        this->textures.push_back(texture);
    }

    glDrawBuffers(this->attachments.size(), this->attachments.data());
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::resize(glm::uvec2 size)
{
    this->size = size;
    for (Texture& texture : this->textures)
    {
        texture.update(Image::loadFromMemory(size, std::vector<float>()));
    }
}

void FrameBuffer::shutdown()
{
    for (Texture& texture : this->textures)
    {
        texture.shutdown();
    }

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
    glBindFramebuffer(GL_FRAMEBUFFER, this->handler);
    glViewport(0, 0, this->size.x, this->size.y);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::stopUse()
{
    glDisable(GL_SCISSOR_TEST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
