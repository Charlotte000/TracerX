/**
 * @file TextureArray.h
 */
#pragma once

#include "TracerX/Image.h"

namespace TracerX::core::GL
{

/**
 * @brief An OpenGL 2D texture array wrapper class.
 * @remark Should not be used directly and is only used internally by the Renderer.
 */
class TextureArray
{
public:
    void init(int internalFormat);
    void bindSampler(unsigned int binding);
    glm::uvec3 getSize() const;
    void update(const std::vector<Image>& images, glm::uvec2 maxSize);
    void shutdown();
private:
    glm::uvec3 size;
    unsigned int handler;
    int internalFormat;

    static glm::uvec2 getMaxSize(const std::vector<Image>& images, glm::uvec2 maxSize);
    static std::vector<float> resizeImages(const std::vector<Image>& images, glm::uvec2 size);
};

}
