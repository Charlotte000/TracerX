#pragma once

#include <string>
#include <SFML/Graphics.hpp>

namespace TracerX
{

template <class T>
class UBO
{
public:
    bool hasChanged = true;

    void create(sf::Shader* shader, const std::string& name);

    void create(sf::Shader* shader, const std::string& name, T value);

    void updateShader();

    const T& get() const;

    void set(const T& value);

private:
    sf::Shader* shader;
    std::string name;
    T value;

    void pushShader();
};

}