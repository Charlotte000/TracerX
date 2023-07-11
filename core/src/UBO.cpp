#include <TracerX/UBO.h>

namespace TracerX
{

template <class T>
void UBO<T>::create(sf::Shader* shader, const std::string& name)
{
    this->shader = shader;
    this->name = name;
    this->pushShader();
}

template <class T>
void UBO<T>::create(sf::Shader* shader, const std::string& name, T value)
{
    this->shader = shader;
    this->name = name;
    this->value = value;
    this->pushShader();
}

template <class T>
void UBO<T>::updateShader()
{
    if (this->hasChanged())
    {
        this->pushShader();
        this->shaderValue = this->value;
    }
}

template <class T>
bool UBO<T>::hasChanged()
{
    return this->value != this->shaderValue;
}

template <class T>
void UBO<T>::pushShader()
{
    this->shader->setUniform(this->name, this->value);
}

template class UBO<int>;
template class UBO<sf::Vector2f>;
template class UBO<sf::Vector3f>;
template class UBO<bool>;
template class UBO<float>;

}