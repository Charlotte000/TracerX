/**
 * @file Environment.cpp
 */
#include "TracerX/Image.h"
#include "TracerX/Environment.h"

using namespace TracerX;

void Environment::loadFromFile(const std::string& fileName)
{
    this->texture.update(Image::loadFromFile(fileName));
}
