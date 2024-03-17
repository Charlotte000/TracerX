#define STB_IMAGE_RESIZE_IMPLEMENTATION

#include "TracerX/TextureArray.h"

#include <stb_image_resize2.h>


void TextureArray::init(glm::ivec2 size, const std::vector<Image>& images)
{
    this->size = size;

    size_t count = size.x * size.y * 3;
    float* data = new float[count * images.size()];
    for (size_t i = 0; i < images.size(); i++)
    {
        const Image& img = images[i];
        stbir_resize_float_linear(img.pixels.data(), img.size.x, img.size.y, 0, data + count * i, size.x, size.y, 0, stbir_pixel_layout::STBIR_RGB);
    }

    glGenTextures(1, &this->handler);
    glBindTexture(GL_TEXTURE_2D_ARRAY, this->handler);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGB32F, size.x, size.y, images.size(), 0, GL_RGB, GL_FLOAT, data);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

    delete[] data;
}

void TextureArray::bind(int binding)
{
    glActiveTexture(GL_TEXTURE0 + binding);
    glBindTexture(GL_TEXTURE_2D_ARRAY, this->handler);
}

void TextureArray::update(glm::ivec2 size, const std::vector<Image>& images)
{
    this->size = size;

    size_t count = size.x * size.y * 3;
    float* data = new float[count * images.size()];
    for (size_t i = 0; i < images.size(); i++)
    {
        const Image& img = images[i];
        stbir_resize_float_linear(img.pixels.data(), img.size.x, img.size.y, 0, data + count * i, size.x, size.y, 0, stbir_pixel_layout::STBIR_RGB);
    }

    glBindTexture(GL_TEXTURE_2D_ARRAY, this->handler);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGB32F, size.x, size.y, images.size(), 0, GL_RGB, GL_FLOAT, data);
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

    delete[] data;
}

void TextureArray::copy(Texture& texture, size_t index)
{
    glCopyImageSubData(this->handler, GL_TEXTURE_2D_ARRAY, 0, 0, 0, index, texture.getHandler(), GL_TEXTURE_2D, 0, 0, 0, 0, this->size.x, this->size.y, 1);
}

void TextureArray::shutdown()
{
    glDeleteTextures(1, &this->handler);
}
