#include <TracerX/UBO.h>

namespace TracerX
{

template <class T>
void UBO<T>::create(sf::Shader* shader, const std::string& name)
{
    this->shader = shader;
    this->name = name;
}

template <class T>
void UBO<T>::create(sf::Shader* shader, const std::string& name, T value)
{
    this->shader = shader;
    this->name = name;
    this->value = value;
}

template <class T>
void UBO<T>::updateShader()
{
    if (this->hasChanged)
    {
        this->pushShader();
    }
}

template <class T>
const T& UBO<T>::get() const
{
    return this->value;
}

template <class T>
void UBO<T>::set(const T& value)
{
    this->value = value;
    this->hasChanged = true;
}

template <class T>
void UBO<T>::pushShader()
{
    this->shader->setUniform(this->name, this->value);
    this->hasChanged = false;
}

template class UBO<int>;
template class UBO<sf::Vector2f>;
template class UBO<sf::Vector3f>;
template class UBO<bool>;
template class UBO<float>;

}