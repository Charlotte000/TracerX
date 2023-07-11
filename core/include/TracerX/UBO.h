#pragma once

#include <string>
#include <SFML/Graphics.hpp>

namespace TracerX
{

template <class T>
class UBO
{
public:
    T value;

    void create(sf::Shader* shader, const std::string& name);

    void create(sf::Shader* shader, const std::string& name, T value);

    void updateShader();

    bool hasChanged();

private:
    sf::Shader* shader;
    std::string name;
    T shaderValue;

    void pushShader();
};

}