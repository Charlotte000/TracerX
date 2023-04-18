#pragma once

#include <SFML/Graphics.hpp>

#include "VectorMath.h"

using namespace sf;

struct Environment
{
    Vector3f skyColorHorizon = Vector3f(.01f, .01f, .01f);
    Vector3f skyColorZenith = Vector3f(.005f, .005f, .01f);
    Vector3f groundColor = Vector3f(.005f, .005f, .005f);
    Vector3f sunColor = Vector3f(1, 1, 1);
    Vector3f sunDirection = normalized(Vector3f(0, -1, -1));
    float sunFocus = 800;
    float sunIntensity = 10;
    bool enabled = true;

    Environment()
    {
    }

    Environment(Vector3f skyColorHorizon, Vector3f skyColorZenith, Vector3f groundColor, Vector3f sunColor, Vector3f sunDirection, float sunFocus, float sunIntensity)
        : skyColorHorizon(skyColorHorizon), skyColorZenith(skyColorZenith), groundColor(groundColor), sunColor(sunColor), sunDirection(normalized(sunDirection)), sunFocus(sunFocus), sunIntensity(sunIntensity)
    {
    }

    void set(Shader& shader, const std::string name)
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
};