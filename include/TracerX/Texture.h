#pragma once

#include "Image.h"

#include <GL/glew.h>


class Texture
{
public:
    int width;
    int height;

    void init(int width, int height);
    void init(const Image& image);
    void bind(int binding);
    void update(const Image& image);
    void update(int width, int height, float* pixels);
    void upload(float* pixels);
    void upload(unsigned char* pixels);
    void shutdown();
    GLuint getHandler() const;
private:
    GLuint handler;
};
