#pragma once

#include <TracerX/RendererVisual.h>

namespace TracerX
{

void InfoUI(RendererVisual& renderer, sf::RenderTexture& target);

void MaterailUI(RendererVisual& renderer);

void GeometryUI(RendererVisual& renderer);

void EnvironmentUI(RendererVisual& renderer);

}