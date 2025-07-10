/**
 * @file Texture.h
 */
#pragma once

#include "TracerX/Image.h"

namespace TracerX::core::GL
{

/**
 * @brief An OpenGL 2D texture wrapper class.
 * 
 * This class represents a texture data stored in GPU memory.
 */
class Texture
{
public:
    /**
     * @brief Initializes the texture with the specified internal format and sampler.
     * @param internalFormat The internal format of the texture, e.g., GL_RGBA32F.
     * @param sampler The sampler to use for the texture, e.g., GL_NEAREST or GL_LINEAR.
     * @remark Should not be used directly and is only used internally by the Renderer.
     */
    void init(int internalFormat, int sampler);

    /**
     * @brief Binds the texture to the specified binding point.
     * @param binding The binding point to bind the texture to, e.g., 0 for texture unit 0.
     * @remark Should not be used directly and is only used internally by the Renderer.
     */
    void bindSampler(unsigned int binding);

    /**
     * @brief Binds the texture as an image to the specified binding point with the specified access.
     * @param binding The binding point to bind the image to, e.g., 0 for image unit 0.
     * @param access The access type for the image, e.g., GL_READ_WRITE or GL_WRITE_ONLY.
     * @remark Should not be used directly and is only used internally by the Renderer.
     */
    void bindImage(unsigned int binding, unsigned int access);

    /**
     * @brief Gets the size of the texture.
     * @return The size of the texture.
     */
    glm::uvec2 getSize() const;

    /**
     * @brief Updates the texture with the specified image.
     * @param image The image to update the texture with.
     */
    void update(const Image& image);

    /**
     * @brief Uploads the texture data from the GPU to an Image object.
     * @return An Image object containing the texture data.
     */
    Image upload() const;

    /**
     * @brief Uploads a rectangular region of the texture from the GPU to an Image object.
     * @param pos The position of the top-left corner of the region.
     * @param size The size of the region.
     * @return An Image object containing the texture data for the specified region.
     */
    Image upload(glm::uvec2 pos, glm::uvec2 size) const;

    /**
     * @brief Fills the texture with black color.
     */
    void clear();

    /**
     * @brief Resizes the texture to the specified size.
     * 
     * This method will clear the texture and allocate new memory for it. 
     * 
     * @param size The new size of the texture.
     */
    void resize(glm::uvec2 size);

    /**
     * @brief Shuts down the texture and releases resources.
     * @remark Should not be used directly and is only used internally by the Renderer.
     */
    void shutdown();

    /**
     * @brief Gets the OpenGL handler for the texture.
     * @return The OpenGL handler for the texture.
     */
    unsigned int getHandler() const;
private:
    glm::uvec2 size;
    unsigned int handler;
    int internalFormat;
};

}
