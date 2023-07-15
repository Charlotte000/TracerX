#pragma once

#include <vector>
#include <SFML/System.hpp>
#include <TracerX/Camera.h>

namespace TracerX
{

struct CameraState
{
    sf::Vector3f position;
    sf::Vector3f forward;
    sf::Vector3f up;
};

struct Animation
{
public:
    float fps = 60.f;
    float duration = 1.f;
    int frameIteration = 100;
    int totalFrames = 0;
    int currentFrame = 0;
    std::string name;

    void load(const std::string& filePath, const std::string& name, Camera& camera);
    
    bool getNextFrame(Camera& camera);

    bool hasLoaded();
private:
    std::vector<CameraState> frames;
};

}