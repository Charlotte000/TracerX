#include <TracerX/VectorMath.h>
#include <TracerX/Environment.h>

namespace TracerX
{

void Environment::create(sf::Shader* shader)
{
    this->skyColorHorizon.create(shader, "Environment.SkyColorHorizon", sf::Vector3f(1, 1, 1));
    this->skyColorZenith.create(shader, "Environment.SkyColorZenith", sf::Vector3f(.5f, .5f, 1));
    this->groundColor.create(shader, "Environment.GroundColor", sf::Vector3f(.5f, .5f, .5f));
    this->sunColor.create(shader, "Environment.SunColor", sf::Vector3f(1, 1, 1));
    this->sunDirection.create(shader, "Environment.SunDirection", normalized(sf::Vector3f(0, -1, -1)));
    this->sunFocus.create(shader, "Environment.SunFocus", 800);
    this->sunIntensity.create(shader, "Environment.SunIntensity", 10);
    this->skyIntensity.create(shader, "Environment.SkyIntensity", .01f);
    this->enabled.create(shader, "Environment.Enabled", true);
}

void Environment::updateShader()
{
    this->skyColorHorizon.updateShader();
    this->skyColorZenith.updateShader();
    this->groundColor.updateShader();
    this->sunColor.updateShader();
    this->sunDirection.updateShader();
    this->sunFocus.updateShader();
    this->sunIntensity.updateShader();
    this->skyIntensity.updateShader();
    this->enabled.updateShader();
}

}