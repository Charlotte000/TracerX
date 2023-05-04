#pragma once

#include <SFML/Graphics.hpp>
#include <TracerX/Renderer.h>

namespace TracerX
{

class RendererVisual : public Renderer
{
public:
    sf::RenderWindow window;
    sf::RenderTexture windowBuffer;
    bool isProgressive = false;
    bool isCameraControl = false;
    bool showCursor = true;

    RendererVisual(sf::Vector2i size, Camera camera, int sampleCount, int maxBounceCount);

    void run();

    void reset();

private:
    sf::RectangleShape cursor;
};

}