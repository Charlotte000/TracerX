#include "TracerX/Quad.h"
#include "TracerX/FrameBuffer.h"


void FrameBuffer::init(int width, int height)
{
    this->colorTexture.init(width, height);

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
    // glActiveTexture(GL_TEXTURE0);
    glBindFramebuffer(GL_FRAMEBUFFER, this->handler);
    glViewport(0, 0, this->colorTexture.width, this->colorTexture.height);
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
