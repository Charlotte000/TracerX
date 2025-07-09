/**
 * @file Environment.cpp
 */
#include "TracerX/Environment.h"

using namespace TracerX;

static float luminance(glm::vec3 c)
{
    return 0.212671f * c.r + 0.715160f * c.g + 0.072169f * c.b;
}

void Environment::update(const Image& image)
{
    this->texture.update(image);
    this->buildCDF(image);
}

void Environment::buildCDF(const Image& image)
{
    const size_t size = image.size.x * image.size.y;
    Image cdf(image.size);

    float sum = 0;
    for (size_t i = 0; i < size; i++)
    {
        sum += luminance(image.get(i));
        cdf.set(i, glm::vec4(sum, 0, 0, 0));
    }

    this->cdfTexture.update(cdf);
    this->cdfTotal = sum;
}
