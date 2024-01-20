#include <TracerX/Renderer.h>
#include <TracerX/Material.h>
#include <TracerX/Primitives/Box.h>
#include <TracerX/Primitives/Mesh.h>
#include <TracerX/Primitives/Sphere.h>
#include <TracerX/Vertex3.h>
#include <TracerX/SSBO.h>

namespace TracerX
{

template <class T>
void SSBO<T>::create(Renderer* renderer, sf::Shader* shader, GLuint binding)
{
    this->renderer = renderer;
    this->shader = shader;
    this->binding = binding;
    glGenBuffers(1, &this->handle);
}

template <class T>
void SSBO<T>::create(Renderer* renderer, sf::Shader* shader, GLuint binding, const std::string& count)
{
    this->renderer = renderer;
    this->shader = shader;
    this->binding = binding;
    this->count = count;
    glGenBuffers(1, &this->handle);
}

template <class T>
void SSBO<T>::updateShader()
{
    if (!this->hasChanged)
    {
        return;
    }

    if (!this->count.empty())
    {
        this->shader->setUniform(this->count, (int)this->values.size());
    }

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->handle);
    glBufferData(GL_SHADER_STORAGE_BUFFER, this->values.size() * sizeof(T), this->values.data(), GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, this->binding, this->handle);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    this->hasChanged = false;
}

template <class T>
const std::vector<T>& SSBO<T>::get() const
{
    return this->values;
}

template <class T>
void SSBO<T>::set(size_t index, const T& value)
{
    this->values[index] = value;
    this->hasChanged = true;
}

template <>
void SSBO<Box>::set(size_t index, const Box& value)
{
    this->values[index] = value;
    this->values[index].updateAABB();
    this->hasChanged = true;
}

template <>
void SSBO<Mesh>::set(size_t index, const Mesh& value)
{
    this->values[index] = value;
    this->values[index].updateAABB(this->renderer->indices, this->renderer->vertices);
    this->hasChanged = true;
}

template <class T>
void SSBO<T>::add(const T& value)
{
    this->values.push_back(value);
    this->hasChanged = true;
}

template <>
void SSBO<Box>::add(const Box& value)
{
    this->values.push_back(value);
    this->values.back().updateAABB();
    this->hasChanged = true;
}

template <>
void SSBO<Mesh>::add(const Mesh& value)
{
    this->values.push_back(value);
    this->values.back().updateAABB(this->renderer->indices, this->renderer->vertices);
    this->hasChanged = true;
}

template <class T>
void SSBO<T>::remove(size_t index)
{
    this->values.erase(this->values.begin() + index);
    this->hasChanged = true;
}

template class SSBO<Material>;
template class SSBO<Sphere>;
template class SSBO<Box>;
template class SSBO<Mesh>;
template class SSBO<int>;
template class SSBO<Vertex3>;

}