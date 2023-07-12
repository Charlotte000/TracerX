#include "GUI/Application.h"
#include "GUI/UI.h"
#include <SFML/System.hpp>
#include <ImGui/imgui.h>
#include <ImGui/imgui-SFML.h>

namespace GUI
{

void Application::create(sf::Vector2i size, const TracerX::Camera& camera, int sampleCount, int maxBounceCount)
{
    this->window.create(sf::VideoMode(size.x, size.y), "Path Tracing");
    Renderer::create(size, camera, sampleCount, maxBounceCount);
    this->windowBuffer.create(size.x, size.y);
    
    this->cursor.setFillColor(sf::Color::Transparent);
    this->cursor.setOutlineColor(sf::Color::Red);
    this->cursor.setOutlineThickness(1);
}

void Application::run()
{
    this->updateMaterials();
    this->updateVertices();
    this->updateSpheres();
    this->updateIndices();
    this->updateMeshes();
    this->updateBoxes();
    this->updateTextures();

    sf::Mouse::setPosition((sf::Vector2i)this->size.get() / 2, this->window);

    if (!ImGui::SFML::Init(this->window))
    {
        throw std::runtime_error("ImGui initialization failed");
    }

    sf::Clock clock;
    while (this->window.isOpen())
    {
        sf::Event event;
        while (this->window.pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(event);

            if (event.type == sf::Event::Closed || event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
            {
                this->window.close();
            }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter && this->isCameraControl)
            {
                this->isCameraControl = false;
                this->window.setMouseCursorVisible(true);
            }
        }

        if (this->isCameraControl)
        {
            this->camera.move(this->window);
        }

        if (!this->isProgressive)
        {
            this->clear();
        }

        // Update UI
        ImGui::SFML::Update(this->window, clock.restart());
        UI(*this);

        // Render frame
        this->renderFrame();
        
        // Draw to window buffer
        sf::Sprite newSprite(this->targetTexture->getTexture());
        newSprite.setTextureRect(this->subFrame);
        newSprite.setPosition((float)this->subFrame.left, (float)this->subFrame.top);
        this->windowBuffer.draw(newSprite);
        this->windowBuffer.display();

        // Draw window
        this->window.clear();
        this->window.draw(sf::Sprite(this->windowBuffer.getTexture()));

        if (this->showCursor)
        {
            this->cursor.setSize(sf::Vector2f((float)this->subFrame.width, (float)this->subFrame.height));
            this->cursor.setPosition((float)this->subFrame.left, (float)this->subFrame.top);
            this->window.draw(this->cursor);
        }

        // Draw UI
        ImGui::SFML::Render(this->window);

        this->window.display(); 
    }

    ImGui::SFML::Shutdown();
}

void Application::reset()
{
    this->frameCount.set(1);
    this->subStage = 0;
    this->isProgressive = false;
}

void Application::clear()
{
    this->buffer1.clear();
    this->buffer1.display();
    this->buffer2.clear();
    this->buffer2.display();
    this->frameCount.set(1);
}

}