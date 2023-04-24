#include <imgui.h>
#include <imgui-SFML.h>

#include "RendererVisual.h"
#include "RendererUI.h"

RendererVisual::RendererVisual(Vector2i size, Camera camera, int sampleCount, int maxBounceCount)
    : Renderer(size, camera, sampleCount, maxBounceCount)
{
    this->window.create(VideoMode(this->size.x, this->size.y), "Ray Tracing");
    this->windowBuffer.create(this->size.x, this->size.y);

    this->cursor.setFillColor(Color::Transparent);
    this->cursor.setOutlineColor(Color::Red);
    this->cursor.setOutlineThickness(1);
}

void RendererVisual::run()
{
    Mouse::setPosition(this->size / 2, this->window);

    ImGui::SFML::Init(this->window);

    Clock clock;
    while (this->window.isOpen())
    {
        Event event;
        while (this->window.pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(event);

            if (event.type == Event::Closed || event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)
            {
                this->window.close();
            }

            if (event.type == Event::KeyPressed && event.key.code == Keyboard::Enter && this->isCameraControl)
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

        this->shader.setUniform("FrameCount", this->frameCount);
        this->shader.setUniform("CameraPosition", this->camera.position);
        this->shader.setUniform("CameraForward", this->camera.forward);
        this->shader.setUniform("CameraUp", this->camera.up);

        ImGui::SFML::Update(this->window, clock.restart());

        InfoUI(*this, this->buffer1);
        MaterailUI(*this);
        GeometryUI(*this);
        EnvironmentUI(*this);

        int subWidth = this->size.x / this->subDivisor.x;
        int subHeight = this->size.y / this->subDivisor.y;
        int y = this->subStage / this->subDivisor.x;
        int x = this->subStage - y * this->subDivisor.x;
        x *= subWidth;
        y *= subHeight;

        RenderTexture* newBuffer = (this->frameCount & 1) == 1 ? &this->buffer1 : &this->buffer2;
        RenderTexture* oldBuffer = (this->frameCount & 1) == 1 ? &this->buffer2 : &this->buffer1;
        Sprite oldSprite(oldBuffer->getTexture());
        Sprite newSprite(newBuffer->getTexture());
        oldSprite.setTextureRect(IntRect(x, y, subWidth, subHeight));
        oldSprite.setPosition((float)x, (float)y);
        newBuffer->draw(oldSprite, &this->shader);
        newBuffer->display();

        newSprite.setTextureRect(IntRect(x, y, subWidth, subHeight));
        newSprite.setPosition((float)x, (float)y);
        this->windowBuffer.draw(newSprite);
        this->windowBuffer.display();

        this->window.clear();
        this->window.draw(Sprite(this->windowBuffer.getTexture()));

        if (this->showCursor)
        {
            this->cursor.setSize(Vector2f((float)subWidth, (float)subHeight));
            this->cursor.setPosition(Vector2f((float)x, (float)y));
            this->window.draw(this->cursor);
        }

        ImGui::SFML::Render(this->window);

        this->window.display();

        this->subStage++;
        if (this->subStage >= this->subDivisor.x * this->subDivisor.y)
        {
            this->subStage = 0;
            this->frameCount++;
        }
    }

    ImGui::SFML::Shutdown();
}

void RendererVisual::reset()
{
    this->frameCount = 1;
    this->subStage = 0;
    this->isProgressive = false;
}