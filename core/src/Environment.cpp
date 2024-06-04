#include "TracerX/Image.h"
#include "TracerX/Environment.h"

using namespace TracerX;

void Environment::reset()
{
    this->name = "None";
    this->texture.update(Image::empty);
}

void Environment::loadFromFile(const std::string& fileName)
{
    this->texture.update(Image::loadFromFile(fileName));
    this->name = fileName.substr(fileName.find_last_of("/\\") + 1);
}
