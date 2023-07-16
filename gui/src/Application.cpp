#include <TracerX/VectorMath.h>
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
            this->cameraControl();
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

void Application::cameraControl()
{
    const float moveSensitivity = 10;

    sf::Vector3f right = this->camera.getRight();
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {
        this->camera.position.set(this->camera.position.get() + this->camera.forward.get() / moveSensitivity);
        this->prevCamera.position.set(this->camera.position.get());
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        this->camera.position.set(this->camera.position.get() - this->camera.forward.get() / moveSensitivity);
        this->prevCamera.position.set(this->camera.position.get());
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        this->camera.position.set(this->camera.position.get() + right / moveSensitivity);
        this->prevCamera.position.set(this->camera.position.get());
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        this->camera.position.set(this->camera.position.get() - right / moveSensitivity);
        this->prevCamera.position.set(this->camera.position.get());
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
    {
        this->camera.position.set(this->camera.position.get() + this->camera.up.get() / moveSensitivity);
        this->prevCamera.position.set(this->camera.position.get());
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
    {
        this->camera.position.set(this->camera.position.get() - this->camera.up.get() / moveSensitivity);
        this->prevCamera.position.set(this->camera.position.get());
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
    {
        this->camera.up.set(TracerX::rotateAroundAxis(this->camera.up.get(), this->camera.forward.get(), 1 / 100.f));
        this->prevCamera.up.set(this->camera.up.get());
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
    {
        this->camera.up.set(TracerX::rotateAroundAxis(this->camera.up.get(), this->camera.forward.get(), -1 / 100.f));
        this->prevCamera.up.set(this->camera.up.get());
    }


    sf::Vector2i center = (sf::Vector2i)window.getSize() / 2;
    sf::Vector2f mouseDelta = ((sf::Vector2f)sf::Mouse::getPosition(window) - (sf::Vector2f)center) / 100.f;
    sf::Mouse::setPosition(center, window);

    this->camera.forward.set(TracerX::rotateAroundAxis(this->camera.forward.get(), right, -mouseDelta.y));
    this->camera.up.set(TracerX::rotateAroundAxis(this->camera.up.get(), right, -mouseDelta.y));

    this->camera.forward.set(TracerX::rotateAroundAxis(this->camera.forward.get(), this->camera.up.get(), -mouseDelta.x));

    this->prevCamera.forward.set(this->camera.forward.get());
    this->prevCamera.up.set(this->camera.up.get());
}

}