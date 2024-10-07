/**
 * @file Image.cpp
 */
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION

#include "TracerX/Image.h"

#include <assert.h>
#include <stdexcept>
#include <stb_image.h>
#include <stb_image_write.h>
#include <stb_image_resize2.h>

using namespace TracerX;

Image Image::empty;

void Image::saveToFile(const std::filesystem::path& path, bool isHDR) const
{
    stbi_flip_vertically_on_write(true);

    if (isHDR)
    {
        // Write to file
        if (!stbi_write_hdr(path.string().c_str(), this->size.x, this->size.y, 4, this->pixels.data()))
        {
            throw std::runtime_error("Failed to save the image");
        }

        return;
    }

    // Convert to unsigned char
    std::vector<unsigned char> data(this->pixels.size());
    for (size_t i = 0; i < this->pixels.size(); i++)
    {
        data[i] = (unsigned char)(this->pixels[i] * 255);
    }

    // Write to file
    if (!stbi_write_png(path.string().c_str(), this->size.x, this->size.y, 4, data.data(), 0))
    {
        throw std::runtime_error("Failed to save the image");
    }
}

Image Image::resize(glm::uvec2 size) const
{
    Image img;
    img.size = size;
    img.pixels.resize(size.x * size.y * 4);
    stbir_resize_float_linear(this->pixels.data(), this->size.x, this->size.y, 0, img.pixels.data(), size.x, size.y, 0, stbir_pixel_layout::STBIR_RGBA);
    return img;
}

Image Image::loadFromFile(const std::filesystem::path& path)
{
    Image img;

    // Read from file
    glm::ivec2 size;
    float* data = stbi_loadf(path.string().c_str(), &size.x, &size.y, nullptr, 4);
    if (data == nullptr)
    {
        throw std::runtime_error("Failed to load the image: " + path.string());
    }

    // Copy to image
    img.size = size;
    size_t pixelCount = img.size.x * img.size.y * 4;
    img.pixels.resize(pixelCount);
    std::copy(data, data + pixelCount, img.pixels.data());

    stbi_image_free(data);
    return img;
}

Image Image::loadFromMemory(glm::uvec2 size, const std::vector<float> pixels)
{
    assert(size.x * size.y * 4 == pixels.size());
    Image img;
    img.size = size;
    img.pixels = pixels;
    return img;
}

Image::Image()
{
}
