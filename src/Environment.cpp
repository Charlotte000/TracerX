#include <TracerX/VectorMath.h>
#include <TracerX/Environment.h>

namespace TracerX
{

Environment::Environment()
{
}

Environment::Environment(sf::Vector3f skyColorHorizon, sf::Vector3f skyColorZenith, sf::Vector3f groundColor, sf::Vector3f sunColor, sf::Vector3f sunDirection, float sunFocus, float sunIntensity, float skyIntensity)
    : skyColorHorizon(skyColorHorizon), skyColorZenith(skyColorZenith), groundColor(groundColor), sunColor(sunColor), sunDirection(normalized(sunDirection)), sunFocus(sunFocus), sunIntensity(sunIntensity), skyIntensity(skyIntensity)
{
}

void Environment::set(sf::Shader& shader) const
{
    shader.setUniform("Environment.SkyColorHorizon", this->skyColorHorizon);
    shader.setUniform("Environment.SkyColorZenith", this->skyColorZenith);
    shader.setUniform("Environment.GroundColor", this->groundColor);
    shader.setUniform("Environment.SunColor", this->sunColor);
    shader.setUniform("Environment.SunDirection", this->sunDirection);
    shader.setUniform("Environment.SunFocus", this->sunFocus);
    shader.setUniform("Environment.SunIntensity", this->sunIntensity);
    shader.setUniform("Environment.SkyIntensity", this->skyIntensity);
    shader.setUniform("Environment.Enabled", this->enabled);
}

}