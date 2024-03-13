#include "TracerX/Quad.h"
#include "TracerX/FrameBuffer.h"


void FrameBuffer::init(glm::ivec2 size)
{
    this->colorTexture.init(size);

    // Create framebuffer
    glGenFramebuffers(1, &this->handler);
    glBindFramebuffer(GL_FRAMEBUFFER, this->handler);

    // Attach color texture
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->colorTexture.getHandler(), 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::shutdown()
{
    this->colorTexture.shutdown();
    glDeleteFramebuffers(1, &this->handler);
}

void FrameBuffer::draw()
{
    glBindFramebuffer(GL_FRAMEBUFFER, this->handler);
    glViewport(0, 0, this->colorTexture.size.x, this->colorTexture.size.y);
    Quad::draw();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::clear()
{
    glBindFramebuffer(GL_FRAMEBUFFER, this->handler);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
