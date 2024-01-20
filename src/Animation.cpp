#include <fstream>
#include <TracerX/Animation.h>

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
        CameraState cameraState;
        file >> cameraState.position.x >> cameraState.position.y >> cameraState.position.z;
        file >> cameraState.forward.x >> cameraState.forward.y >> cameraState.forward.z;
        file >> cameraState.up.x >> cameraState.up.y >> cameraState.up.z;
        file >> cameraState.focalLength;
        file >> cameraState.focusStrength;
        file >> cameraState.fov;
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

    CameraState prevCameraState = this->frames[std::max(0, this->currentFrame - 1)];
    CameraState cameraState = this->frames[this->currentFrame];

    prevCamera.position.set(prevCameraState.position);
    prevCamera.forward.set(prevCameraState.forward);
    prevCamera.up.set(prevCameraState.up);
    prevCamera.focalLength.set(prevCameraState.focalLength);
    prevCamera.focusStrength.set(prevCameraState.focusStrength);
    prevCamera.fov.set(prevCameraState.fov);

    camera.position.set(cameraState.position);
    camera.forward.set(cameraState.forward);
    camera.up.set(cameraState.up);
    camera.focalLength.set(cameraState.focalLength);
    camera.focusStrength.set(cameraState.focusStrength);
    camera.fov.set(cameraState.fov);
    return true;
}

bool Animation::hasLoaded()
{
    return this->frames.size() > 0;
}

}