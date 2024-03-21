#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION

#include "TracerX/Image.h"

#include <iostream>
#include <stdexcept>
#include <stb_image.h>
#include <stb_image_write.h>
#include <stb_image_resize2.h>


Image Image::empty;

void Image::saveToFile() const
{
    std::vector<unsigned char> data(this->pixels.begin(), this->pixels.end());
    stbi_flip_vertically_on_write(true);
    if (!stbi_write_png(this->name.c_str(), this->size.x, this->size.y, 3, data.data(), 0))
    {
        std::cerr << "Failed to save the image" << std::endl;
    }
}

Image Image::resize(glm::ivec2 size) const
{
    std::vector<float> newPixels(size.x * size.y * 3);
    stbir_resize_float_linear(this->pixels.data(), this->size.x, this->size.y, 0, newPixels.data(), size.x, size.y, 0, stbir_pixel_layout::STBIR_RGB);
    return Image::loadFromMemory(this->name, size, newPixels);
}

Image Image::loadFromFile(const std::string& fileName)
{
    Image img;
    float* data = stbi_loadf(fileName.c_str(), &img.size.x, &img.size.y, nullptr, 3);
    if (data == nullptr)
    {
        throw std::runtime_error("Failed to load the image: " + fileName);
    }

    size_t size = img.size.x * img.size.y * 3;
    img.pixels.resize(size);
    std::copy(data, data + size, img.pixels.data());

    img.name = fileName.substr(fileName.find_last_of("/\\") + 1);

    stbi_image_free(data);
    return img;
}

Image Image::loadFromMemory(const std::string& name, glm::ivec2 size, const std::vector<unsigned char> pixels)
{
    Image img;
    img.name = name;
    img.size = size;
    img.pixels = std::vector<float>(pixels.begin(), pixels.end());
    return img;
}

Image Image::loadFromMemory(const std::string& name, glm::ivec2 size, const std::vector<float> pixels)
{
    Image img;
    img.name = name;
    img.size = size;
    img.pixels = pixels;
    return img;
}

Image::Image()
{
}
