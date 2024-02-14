#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "TracerX/Image.h"

#include <iostream>
#include <stdexcept>
#include <stb_image.h>
#include <stb_image_write.h>


Image Image::empty;

Image Image::loadFromFile(const std::string& fileName)
{
    Image img;
    float* data = stbi_loadf(fileName.c_str(), &img.width, &img.height, nullptr, 3);
    if (data == nullptr)
    {
        throw std::runtime_error("Failed to load the image: " + fileName);
    }

    size_t size = img.width * img.height * 3;
    img.pixels.resize(size);
    std::copy(data, data + size, img.pixels.data());

    img.name = fileName.substr(fileName.find_last_of("/\\") + 1);

    stbi_image_free(data);
    return img;
}

void Image::saveToDisk(const std::string& fileName, int width, int height, unsigned char* pixels)
{
    stbi_flip_vertically_on_write(true);
    if (!stbi_write_png(fileName.c_str(), width, height, 3, pixels, 0))
    {
        std::cerr << "Failed to save the image" << std::endl;
    }
}

Image::Image()
{
}
