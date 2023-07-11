#pragma once

#include <SFML/System.hpp>
#include <TracerX/Renderer.h>

namespace GUI
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
    
private:
    sf::RenderTexture windowBuffer;
    sf::RectangleShape cursor;
};

}