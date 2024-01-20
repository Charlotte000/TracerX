#pragma once

#include <SFML/System.hpp>
#include "Renderer.h"

namespace TracerX
{

class Application : public TracerX::Renderer
{
public:
    bool isProgressive = false;
    bool isCameraControl = false;
    bool showCursor = true;
    sf::RenderWindow window;

    void create(sf::Vector2i size, const TracerX::Camera& camera, int sampleCount, int maxBounceCount);

    void run();

    void reset();

    void clear();

    void cameraControl();
    
private:
    sf::RenderTexture windowBuffer;
    sf::RectangleShape cursor;
};

}