#include <fstream>
#include <TracerX/Animation.h>
#include <TracerX/Serialization.h>

namespace TracerX
{

void Animation::load(const std::string& filePath, const std::string& name, Camera& prevCamera, Camera& camera)
{
    std::ifstream file;
    file.open(filePath);
    if (!file.is_open())
    {
        throw std::runtime_error("File not found");
    }

    this->frames.clear();
    while (!file.eof())
    {
        Camera cameraState;
        file >> cameraState;
        this->frames.push_back(cameraState);
    }

    file.close();

    this->name = name;
    this->currentFrame = -1;
    this->totalFrames = std::min((int)(this->fps * this->duration), (int)this->frames.size());
    this->getNextFrame(prevCamera, camera);
}

bool Animation::getNextFrame(Camera& prevCamera, Camera& camera)
{
    if (this->currentFrame + 1 >= this->totalFrames)
    {
        return false;
    }
    
    this->currentFrame++;

    Camera prevCameraState = this->frames[std::max(0, this->currentFrame - 1)];
    Camera cameraState = this->frames[this->currentFrame];

    prevCamera.set(prevCameraState);
    camera.set(cameraState);
    return true;
}

bool Animation::hasLoaded()
{
    return this->frames.size() > 0;
}

}