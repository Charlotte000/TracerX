#pragma once

#include <vector>
#include <SFML/System.hpp>
#include "Camera.h"

namespace TracerX
{

struct Animation
{
public:
    float fps = 60.f;
    float duration = 1.f;
    int frameIteration = 100;
    int totalFrames = 0;
    int currentFrame = 0;
    std::string name;

    void load(const std::string& filePath, const std::string& name, Camera& prevCamera, Camera& camera);
    
    bool getNextFrame(Camera& prevCamera, Camera& camera);

    bool hasLoaded();
private:
    std::vector<Camera> frames;
};

}