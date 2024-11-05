/**
 * @file Environment.cpp
 */
#include "TracerX/Environment.h"

using namespace TracerX;

void Environment::loadFromFile(const std::filesystem::path& path)
{
    this->texture.update(Image::loadFromFile(path));
}

void Environment::loadFromImage(const Image& image)
{
    this->texture.update(image);
}
