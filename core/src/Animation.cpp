#include <fstream>
#include <TracerX/Animation.h>

namespace TracerX
{

void Animation::load(const std::string& filePath, const std::string& name, Camera& camera)
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
        CameraState cameraState;
        file >> cameraState.position.x >> cameraState.position.y >> cameraState.position.z;
        file >> cameraState.forward.x >> cameraState.forward.y >> cameraState.forward.z;
        file >> cameraState.up.x >> cameraState.up.y >> cameraState.up.z;
        this->frames.push_back(cameraState);
    }

    file.close();

    this->name = name;
    this->currentFrame = -1;
    this->totalFrames = std::min((int)(this->fps * this->duration), (int)this->frames.size());
    this->getNextFrame(camera);
}

bool Animation::getNextFrame(Camera& camera)
{
    if (this->currentFrame + 1 > this->totalFrames)
    {
        return false;
    }
    
    this->currentFrame++;

    camera.prevPosition.set(camera.position.get());
    camera.prevForward.set(camera.forward.get());
    camera.prevUp.set(camera.up.get());

    CameraState cameraState = this->frames[this->currentFrame];
    camera.position.set(cameraState.position);
    camera.forward.set(cameraState.forward);
    camera.up.set(cameraState.up);
    return true;
}

bool Animation::hasLoaded()
{
    return this->frames.size() > 0;
}

}