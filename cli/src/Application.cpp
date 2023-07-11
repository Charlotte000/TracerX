#include "CLI/Application.h"
#include <iostream>

namespace CLI
{

void Application::create(sf::Vector2i size, const TracerX::Camera& camera, int sampleCount, int maxBounceCount)
{
    this->window.create(sf::VideoMode(size.x, size.y), "Path Tracing");
    this->window.setVisible(false);
    Renderer::create(size, camera, sampleCount, maxBounceCount);
}

void Application::run(int iterationCount, const std::string& imagePath)
{
    this->updateMaterials();
    this->updateVertices();
    this->updateSpheres();
    this->updateIndices();
    this->updateMeshes();
    this->updateBoxes();
    this->updateTextures();

    int subWidth = this->size.x / this->subDivisor.x;
    int subHeight = this->size.y / this->subDivisor.y;
    int subCount = this->subDivisor.x * this->subDivisor.y;
    while (this->frameCount <= iterationCount)
    {
        // Update shader parameters
        this->shader.setUniform("FrameCount", this->frameCount);

        this->renderFrame();

        // Print progress
        static const int barWidth = 50;
        float progress = (float)(this->subStage) / subCount;
        int progressWidth = (int)(progress * barWidth);
        std::cout << std::nounitbuf;
        std::cout << "\x1b[F[" << std::string(progressWidth, '#');
        std::cout << std::string(barWidth - progressWidth, ' ') << "] ";
        std::cout << (int)(progress * 100) << "%  " << std::endl;

        progress = (float)(this->frameCount - 1) / iterationCount;
        progressWidth = (int)(progress * barWidth);
        std::cout << '[' << std::string(progressWidth, '#');
        std::cout << std::string(barWidth - progressWidth, ' ') << "] ";
        std::cout << (int)(progress * 100) << '%';
        std::cout << std::flush;
    }

    std::cout << std::endl;

    this->targetTexture->getTexture().copyToImage().saveToFile(imagePath);
}

}