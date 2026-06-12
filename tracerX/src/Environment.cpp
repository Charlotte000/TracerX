/**
 * @file Environment.cpp
 */
#include "TracerX/Environment.h"

using namespace TracerX;

static float luminance(glm::vec3 c)
{
    return 0.212671f * c.r + 0.715160f * c.g + 0.072169f * c.b;
}

void Environment::update(const OGL::Image2D& image)
{
    this->texture = OGL::Texture2D(image, OGL::ImageFormat::RGBA32F);
    this->buildCDF(image);
}

void Environment::buildCDF(const OGL::Image2D& image)
{
    const size_t size = image.size.x * image.size.y;
    OGL::Image2D cdf(image.size);

    float sum = 0;
    for (size_t i = 0; i < size; i++)
    {
        sum += luminance(image.pixels[i]);
        cdf.pixels[i] = glm::vec4(sum, 0, 0, 0);
    }

    this->cdfTexture = OGL::Texture2D(image.size, OGL::ImageFormat::R32F);
    this->cdfTexture.update(cdf, glm::uvec2(0));
    this->cdfTotal = sum;
}
