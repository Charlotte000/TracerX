#define STB_IMAGE_RESIZE_IMPLEMENTATION

#include "TracerX/TextureArray.h"

#include <stb_image_resize2.h>


void TextureArray::init(int width, int height, const std::vector<Image>& images)
{
    this->width = width;
    this->height = height;

    size_t size = this->width * this->height * 3;
    float* data = new float[size * images.size()];
    for (size_t i = 0; i < images.size(); i++)
    {
        const Image& img = images[i];
        stbir_resize_float_linear(img.pixels.data(), img.width, img.height, 0, data + size * i, this->width, this->height, 0, stbir_pixel_layout::STBIR_RGB);
    }

    glGenTextures(1, &this->handler);
    glBindTexture(GL_TEXTURE_2D_ARRAY, this->handler);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGB32F, this->width, this->height, images.size(), 0, GL_RGB, GL_FLOAT, data);
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

void TextureArray::update(int width, int height, const std::vector<Image>& images)
{
    this->width = width;
    this->height = height;

    size_t size = this->width * this->height * 3;
    float* data = new float[size * images.size()];
    for (size_t i = 0; i < images.size(); i++)
    {
        const Image& img = images[i];
        stbir_resize_float_linear(img.pixels.data(), img.width, img.height, 0, data + size * i, this->width, this->height, 0, stbir_pixel_layout::STBIR_RGB);
    }

    glBindTexture(GL_TEXTURE_2D_ARRAY, this->handler);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGB32F, this->width, this->height, images.size(), 0, GL_RGB, GL_FLOAT, data);
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

void TextureArray::shutdown()
{
    glDeleteTextures(1, &this->handler);
}
