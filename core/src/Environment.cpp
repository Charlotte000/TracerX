/**
 * @file Environment.cpp
 */
#include "TracerX/Image.h"
#include "TracerX/Environment.h"

using namespace TracerX;

void Environment::loadFromFile(const std::filesystem::path& path)
{
    this->texture.update(Image::loadFromFile(path));
}
