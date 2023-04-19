#pragma once

#include "Renderer.h"

void InfoUI(Renderer& renderer, bool& isProgressive, bool& isCameraControl, sf::RenderWindow& window, sf::RenderTexture& target);

void MaterailUI(Renderer& renderer, bool& isProgressive);

void GeometryUI(Renderer& renderer, bool& isProgressive);

void EnvironmentUI(Renderer& renderer, bool& isProgressive);