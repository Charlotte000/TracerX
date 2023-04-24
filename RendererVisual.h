#pragma once

#include "Renderer.h"

class RendererVisual : public Renderer
{
public:
    RenderWindow window;
    RenderTexture windowBuffer;
    bool isProgressive = false;
    bool isCameraControl = false;
    bool showCursor = true;
    RectangleShape cursor;

    RendererVisual(Vector2i size, Camera camera, int sampleCount, int maxBounceCount);

    void run();

    void reset();
};