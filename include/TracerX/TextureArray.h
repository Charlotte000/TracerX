#pragma once

#include "Image.h"

#include <vector>
#include <GL/glew.h>


class TextureArray
{
public:
    int width;
    int height;

    void init(int width, int height, const std::vector<Image>& images);
    void bind(int binding);
    void update(int width, int height, const std::vector<Image>& images);
    void shutdown();
private:
    GLuint handler;
};