#pragma once

#include <SFML/Graphics.hpp>
#include <TracerX/Primitives/Box.h>
#include <TracerX/Primitives/Sphere.h>
#include <TracerX/Camera.h>
#include <TracerX/Environment.h>
#include <TracerX/Vertex3.h>
#include <TracerX/Primitives/Mesh.h>
#include <TracerX/Material.h>
#include <TracerX/SSBO.h>
#include <TracerX/UBO.h>

namespace TracerX
{

class Renderer
{
public:
    // SSBO
    SSBO<Material> materials;
    SSBO<Sphere> spheres;
    SSBO<Box> boxes;
    SSBO<int> indices;
    SSBO<Vertex3> vertices;
    SSBO<Mesh> meshes;

    std::vector<sf::Texture> textures;

    // UBO
    UBO<sf::Vector2f> size;
    UBO<int> frameCount;
    UBO<int> sampleCount;
    UBO<int> maxBounceCount;
    Camera camera;
    Environment environment;

    // Non shader 
    int subStage = 0;
    sf::Vector2i subDivisor = sf::Vector2i(1, 1);
    sf::RenderTexture* targetTexture;

    void create(sf::Vector2i size, const Camera& camera, int sampleCount, int maxBounceCount);

    void renderFrame();

    int getPixelDifference() const;

    void add(const Sphere& sphere, const Material& material);

    void add(const Sphere& sphere);

    void add(const Box& box, const Material& material);

    void add(const Box& box);

    int add(const Material& material);

    int addTexture(const std::string& filePath);

    void addFile(const std::string& filePath, sf::Vector3f offset = sf::Vector3f(0, 0, 0), sf::Vector3f scale = sf::Vector3f(1, 1, 1), sf::Vector3f rotation = sf::Vector3f(0, 0, 0));

    void addCornellBox(const Material& up, const Material& down, const Material& left, const Material& right, const Material& forward, const Material& backward, const Material& lightSource);

    void addCornellBox(const Material& up, const Material& down, const Material& left, const Material& right, const Material& forward, const Material& backward);

    void updateTextures();

protected:
    sf::RenderTexture buffer1;
    sf::RenderTexture buffer2;
    sf::RenderTexture* bufferTargetTexture;
    sf::IntRect subFrame;

    void loadShader();

private:
    static const std::string ShaderCode;
    sf::Shader shader;
};

}