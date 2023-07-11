#pragma once

#include <TracerX/Renderer.h>
#include <string>

namespace CLI
{

class Application : public TracerX::Renderer
{
public:
    sf::RenderWindow window;

    void create(sf::Vector2i size, const TracerX::Camera& camera, int sampleCount, int maxBounceCount);

    void run(int iterationCount, const std::string& imagePath);
};

}