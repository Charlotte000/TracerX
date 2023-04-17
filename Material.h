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

    bool isTransparent;
    float refractionFactor;

    Material(Vector3f albedoColor = Vector3f(0, 0, 0), float roughness = 0, Vector3f metalnessColor = Vector3f(0, 0, 0), float metalness = 0, Vector3f emissionColor = Vector3f(0, 0, 0), float emissionStrength = 0, Vector3f fresnelColor = Vector3f(0, 0, 0), float fresnelStrength = 0, bool isTransparent = false, float refractionFactor = 1)
        : albedoColor(albedoColor), roughness(roughness), emissionColor(emissionColor), emissionStrength(emissionStrength), metalnessColor(metalnessColor), metalness(metalness), fresnelColor(fresnelColor), fresnelStrength(fresnelStrength), isTransparent(isTransparent), refractionFactor(refractionFactor)
    {
    }

    static Material LightSource(Vector3f color, float strength)
    {
        Material m;
        m.emissionColor = color;
        m.emissionStrength = strength;
        return m;
    }

    static Material Transparent(Vector3f albedoColor, float refractionFactor, Vector3f fresnelColor, float fresnelStrength)
    {
        Material m(albedoColor, 1);
        m.isTransparent = true;
        m.refractionFactor = refractionFactor;
        m.fresnelColor = fresnelColor;
        m.fresnelStrength = fresnelStrength;
        return m;
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
            a.fresnelStrength == this->fresnelStrength &&
            a.isTransparent == this->isTransparent &&
            a.refractionFactor == this->refractionFactor;
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
        shader.setUniform(name + ".IsTransparent", this->isTransparent);
        shader.setUniform(name + ".RefractionFactor", this->refractionFactor);
    }
};