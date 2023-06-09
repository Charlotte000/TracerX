#pragma once

#include <vector>
#include <string>
#include <GL/glew.h>
#include <SFML/Graphics.hpp>

namespace TracerX
{

class Renderer;

template <class T>
class SSBO
{
public:
    void create(Renderer* renderer, sf::Shader* shader, GLuint binding);

    void create(Renderer* renderer, sf::Shader* shader, GLuint binding, const std::string& count);

    void updateShader();

    const std::vector<T>& get() const;

    void set(size_t index, const T& value);

    void add(const T& value);

    void remove(size_t index);

private:
    Renderer* renderer;
    sf::Shader* shader;
    std::string count;
    std::vector<T> values;
    GLuint binding;
    GLuint handle;
    bool hasChanged = true;
};

}