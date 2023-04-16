#pragma once

#include <SFML/Graphics.hpp>

using namespace sf;

struct Material
{
    Vector3f albedoColor;
    float roughness;
    Vector3f metalnessColor;
    float metalness;

    Vector3f emissionColor;
    float emissionStrength;
    
    Vector3f fresnelColor;
    float fresnelStrength;

    Material(Vector3f albedoColor, float roughness = 0, Vector3f metalnessColor = Vector3f(0, 0, 0), float metalness = 0, Vector3f emissionColor = Vector3f(0, 0, 0), float emissionStrength = 0, Vector3f fresnelColor = Vector3f(0, 0, 0), float fresnelStrength = 0)
        : albedoColor(albedoColor), roughness(roughness), emissionColor(emissionColor), emissionStrength(emissionStrength), metalnessColor(metalnessColor), metalness(metalness), fresnelColor(fresnelColor), fresnelStrength(fresnelStrength)
    {
    }

    bool operator==(const Material& a) const
    {
        return a.albedoColor == this->albedoColor &&
            a.roughness == this->roughness &&
            a.emissionColor == this->emissionColor &&
            a.emissionStrength == this->emissionStrength &&
            a.metalnessColor == this->metalnessColor &&
            a.metalness == this->metalness &&
            a.fresnelColor == this->fresnelColor &&
            a.fresnelStrength == this->fresnelStrength;
    }

    void set(Shader &shader, const std::string name)
    {
        shader.setUniform(name + ".AlbedoColor", this->albedoColor);
        shader.setUniform(name + ".Roughness", this->roughness);
        shader.setUniform(name + ".EmissionColor", this->emissionColor);
        shader.setUniform(name + ".EmissionStrength", this->emissionStrength);
        shader.setUniform(name + ".MetalnessColor", this->metalnessColor);
        shader.setUniform(name + ".Metalness", this->metalness);
        shader.setUniform(name + ".FresnelColor", this->fresnelColor);
        shader.setUniform(name + ".FresnelStrength", this->fresnelStrength);
    }
};