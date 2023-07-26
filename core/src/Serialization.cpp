#include <TracerX/Serialization.h>

namespace TracerX
{

std::ifstream& operator >>(std::ifstream& input, sf::Vector2f& vector)
{
    input >> vector.x >> vector.y;
    return input;
}

std::ofstream& operator <<(std::ofstream& output, const sf::Vector2f& vector)
{
    output << vector.x << ' ' << vector.y;
    return output;
}

std::ifstream& operator >>(std::ifstream& input, sf::Vector3f& vector)
{
    input >> vector.x >> vector.y >> vector.z;
    return input;
}

std::ofstream& operator <<(std::ofstream& output, const sf::Vector3f& vector)
{
    output << vector.x << ' ' << vector.y << ' ' << vector.z;
    return output;
}

std::ifstream& operator >>(std::ifstream& input, Material& material)
{
    input >> material.albedoColor;
    input >> material.roughness;
    input >> material.metalnessColor >> material.metalness;
    input >> material.emissionColor >> material.emissionStrength;
    input >> material.fresnelColor >> material.fresnelStrength;
    input >> material.refractionFactor;
    input >> material.density;
    return input;
}

std::ofstream& operator <<(std::ofstream& output, const Material& material)
{
    output << material.albedoColor << ' ';
    output << material.roughness << ' ';
    output << material.metalnessColor << ' ' << material.metalness << ' ';
    output << material.emissionColor << ' ' << material.emissionStrength << ' ';
    output << material.fresnelColor << ' ' << material.fresnelStrength << ' ';
    output << material.refractionFactor << ' ';
    output << material.density;
    return output;
}

std::ifstream& operator >>(std::ifstream& input, Sphere& sphere)
{
    input >> sphere.origin;
    input >> sphere.radius;
    input >> sphere.rotation;
    input >> sphere.materialId;
    return input;
}

std::ofstream& operator <<(std::ofstream& output, const Sphere& sphere)
{
    output << sphere.origin << ' ';
    output << sphere.radius << ' ';
    output << sphere.rotation << ' ';
    output << sphere.materialId;
    return output;
}

std::ifstream& operator >>(std::ifstream& input, Box& box)
{
    input >> box.origin;
    input >> box.size;
    input >> box.rotation;
    input >> box.materialId;
    return input;
}

std::ofstream& operator <<(std::ofstream& output, const Box& box)
{
    output << box.origin << ' ';
    output << box.size << ' ';
    output << box.rotation << ' ';
    output << box.materialId;
    return output;
}

std::ifstream& operator >>(std::ifstream& input, Mesh& mesh)
{
    input >> mesh.position;
    input >> mesh.size;
    input >> mesh.rotation;
    input >> mesh.indicesStart;
    input >> mesh.indicesEnd;
    input >> mesh.materialId;
    return input;
}

std::ofstream& operator <<(std::ofstream& output, const Mesh& mesh)
{
    output << mesh.position << ' ';
    output << mesh.size << ' ';
    output << mesh.rotation << ' ';
    output << mesh.indicesStart << ' ';
    output << mesh.indicesEnd << ' ';
    output << mesh.materialId;
    return output;
}

std::ifstream& operator >>(std::ifstream& input, Vertex3& vertex)
{
    input >> vertex.position;
    input >> vertex.normal;
    input >> vertex.textureCoordinate;
    return input;
}

std::ofstream& operator <<(std::ofstream& output, const Vertex3& vertex)
{
    output << vertex.position << ' ';
    output << vertex.normal << ' ';
    output << vertex.textureCoordinate;
    return output;
}

std::ifstream& operator >>(std::ifstream& input, Camera& camera)
{
    sf::Vector3f vector;
    float value;

    input >> vector;
    camera.position.set(vector);

    input >> vector;
    camera.forward.set(vector);

    input >> vector;
    camera.up.set(vector);
    
    input >> value;
    camera.focalLength.set(value);
    
    input >> value;
    camera.focusStrength.set(value);
    
    input >> value;
    camera.fov.set(value);
    return input;
}

std::ofstream& operator <<(std::ofstream& output, const Camera& camera)
{
    output << camera.position.get() << ' ';
    output << camera.forward.get() << ' ';
    output << camera.up.get() << ' ';
    output << camera.focalLength.get() << ' ';
    output << camera.focusStrength.get() << ' ';
    output << camera.fov.get();
    return output;
}

std::ifstream& operator >>(std::ifstream& input, Environment& environment)
{
    bool boolValue;
    float floatValue;
    sf::Vector3f vectorValue;

    input >> vectorValue;
    environment.skyColorHorizon.set(vectorValue);

    input >> vectorValue;
    environment.skyColorZenith.set(vectorValue);

    input >> vectorValue;
    environment.groundColor.set(vectorValue);

    input >> vectorValue;
    environment.sunColor.set(vectorValue);

    input >> vectorValue;
    environment.sunDirection.set(vectorValue);

    input >> floatValue;
    environment.sunFocus.set(floatValue);

    input >> floatValue;
    environment.sunIntensity.set(floatValue);

    input >> floatValue;
    environment.skyIntensity.set(floatValue);

    input >> boolValue;
    environment.enabled.set(boolValue);
    return input;
}

std::ofstream& operator <<(std::ofstream& output, const Environment& environment)
{
    output << environment.skyColorHorizon.get() << ' ';
    output << environment.skyColorZenith.get() << ' ';
    output << environment.groundColor.get() << ' ';
    output << environment.sunColor.get() << ' ';
    output << environment.sunDirection.get() << ' ';
    output << environment.sunFocus.get() << ' ';
    output << environment.sunIntensity.get() << ' ';
    output << environment.skyIntensity.get() << ' ';
    output << environment.enabled.get();
    return output;
}

}