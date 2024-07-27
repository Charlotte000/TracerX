/**
 * @file TextureArray.cpp
 */
#include "TracerX/TextureArray.h"

using namespace TracerX::core;

void TextureArray::init(GLint internalFormat)
{
    this->internalFormat = internalFormat;
    glGenTextures(1, &this->handler);
    glBindTexture(GL_TEXTURE_2D_ARRAY, this->handler);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

void TextureArray::bind(int binding)
{
    glBindTextureUnit(binding, this->handler);
}

void TextureArray::update(const std::vector<Image>& images)
{
    glm::uvec2 size = TextureArray::getMaxSize(images);
    std::vector<float> data = TextureArray::resizeImages(images, size);

    glBindTexture(GL_TEXTURE_2D_ARRAY, this->handler);
    {
        glm::uvec3 size3(size, images.size());
        if (this->size != size3)
        {
            this->size = size3;
            glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, this->internalFormat, this->size.x, this->size.y, this->size.z, 0, GL_RGBA, GL_FLOAT, data.data());
        }
        else
        {
            glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, this->size.x, this->size.y, this->size.z, GL_RGBA, GL_FLOAT, data.data());
        }
    }
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

void TextureArray::shutdown()
{
    glDeleteTextures(1, &this->handler);
}

glm::uvec2 TextureArray::getMaxSize(const std::vector<Image>& images)
{
    glm::uvec2 size(0, 0);
    for (const Image& image : images)
    {
        size = glm::max(size, image.size);
    }

    return size;
}

std::vector<float> TextureArray::resizeImages(const std::vector<Image>& images, glm::uvec2 size)
{
    size_t count = size.x * size.y * 4;
    std::vector<float> data(count * images.size());
    for (size_t i = 0; i < images.size(); i++)
    {
        const Image resizedImg = images[i].resize(size);
        data.insert(data.begin() + count * i, resizedImg.pixels.begin(), resizedImg.pixels.end());
    }

    return data;   
}