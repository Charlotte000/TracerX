#pragma once

#include <TracerX/Renderer.h>

namespace TracerX
{

void InfoUI(Renderer& renderer, sf::RenderTexture& target);

void MaterialUI(Renderer& renderer);

void GeometryUI(Renderer& renderer);

void EnvironmentUI(Renderer& renderer);

void TextureUI(Renderer& renderer);

}