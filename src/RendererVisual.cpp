#include <ImGui/imgui.h>
#include <ImGui/imgui-SFML.h>
#include <TracerX/RendererVisual.h>
#include <TracerX/RendererUI.h>
#include <stdexcept>

namespace TracerX
{

RendererVisual::RendererVisual(sf::Vector2i size, Camera camera, int sampleCount, int maxBounceCount)
    : Renderer(size, camera, sampleCount, maxBounceCount)
{
    this->window.create(sf::VideoMode(this->size.x, this->size.y), "Ray Tracing");
    this->windowBuffer.create(this->size.x, this->size.y);

    this->cursor.setFillColor(sf::Color::Transparent);
    this->cursor.setOutlineColor(sf::Color::Red);
    this->cursor.setOutlineThickness(1);
}

void RendererVisual::run()
{
    this->loadShader();

    sf::Mouse::setPosition(this->size / 2, this->window);

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

        this->shader.setUniform("FrameCount", this->frameCount);
        this->shader.setUniform("CameraPosition", this->camera.position);
        this->shader.setUniform("CameraForward", this->camera.forward);
        this->shader.setUniform("CameraUp", this->camera.up);

        ImGui::SFML::Update(this->window, clock.restart());

        ImGui::BeginMainMenuBar();
        if (ImGui::BeginMenu("Info"))
        {
            InfoUI(*this, this->buffer1);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Material"))
        {
            MaterialUI(*this);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Geometry"))
        {
            GeometryUI(*this);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Environment"))
        {
            EnvironmentUI(*this);
            ImGui::EndMenu();
        }

        ImGui::Separator();

        ImGui::Text("FPS: %i", (int)roundf(ImGui::GetIO().Framerate));

        ImGui::EndMainMenuBar();

        int subWidth = this->size.x / this->subDivisor.x;
        int subHeight = this->size.y / this->subDivisor.y;
        int y = this->subStage / this->subDivisor.x;
        int x = this->subStage - y * this->subDivisor.x;
        x *= subWidth;
        y *= subHeight;

        sf::RenderTexture* newBuffer = (this->frameCount & 1) == 1 ? &this->buffer1 : &this->buffer2;
        sf::RenderTexture* oldBuffer = (this->frameCount & 1) == 1 ? &this->buffer2 : &this->buffer1;
        sf::Sprite oldSprite(oldBuffer->getTexture());
        sf::Sprite newSprite(newBuffer->getTexture());
        oldSprite.setTextureRect(sf::IntRect(x, y, subWidth, subHeight));
        oldSprite.setPosition((float)x, (float)y);
        newBuffer->draw(oldSprite, &this->shader);
        newBuffer->display();

        newSprite.setTextureRect(sf::IntRect(x, y, subWidth, subHeight));
        newSprite.setPosition((float)x, (float)y);
        this->windowBuffer.draw(newSprite);
        this->windowBuffer.display();

        this->window.clear();
        this->window.draw(sf::Sprite(this->windowBuffer.getTexture()));

        if (this->showCursor)
        {
            this->cursor.setSize(sf::Vector2f((float)subWidth, (float)subHeight));
            this->cursor.setPosition(sf::Vector2f((float)x, (float)y));
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

}