#include <TracerX/Environment.h>
#include <TracerX/VectorMath.h>

namespace TracerX
{

Environment::Environment()
{
}

Environment::Environment(sf::Vector3f skyColorHorizon, sf::Vector3f skyColorZenith, sf::Vector3f groundColor, sf::Vector3f sunColor, sf::Vector3f sunDirection, float sunFocus, float sunIntensity)
    : skyColorHorizon(skyColorHorizon), skyColorZenith(skyColorZenith), groundColor(groundColor), sunColor(sunColor), sunDirection(normalized(sunDirection)), sunFocus(sunFocus), sunIntensity(sunIntensity)
{
}

void Environment::set(sf::Shader& shader, const std::string name)
{
    shader.setUniform(name + ".SkyColorHorizon", this->skyColorHorizon);
    shader.setUniform(name + ".SkyColorZenith", this->skyColorZenith);
    shader.setUniform(name + ".GroundColor", this->groundColor);
    shader.setUniform(name + ".SunColor", this->sunColor);
    shader.setUniform(name + ".SunDirection", this->sunDirection);
    shader.setUniform(name + ".SunFocus", this->sunFocus);
    shader.setUniform(name + ".SunIntensity", this->sunIntensity);
    shader.setUniform(name + ".Enabled", this->enabled);
}

}