#pragma once

#include <SFML/Graphics.hpp>
#include "Primitives/Box.h"
#include "Primitives/Sphere.h"
#include "Primitives/Mesh.h"
#include "Camera.h"
#include "Environment.h"
#include "Vertex3.h"
#include "Material.h"
#include "SSBO.h"
#include "UBO.h"
#include "Animation.h"

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
    Camera prevCamera;
    Environment environment;

    // Non shader 
    int subStage = 0;
    sf::Vector2i subDivisor = sf::Vector2i(1, 1);
    sf::RenderTexture* targetTexture;
    Animation animation;

    void create(sf::Vector2i size, const Camera& camera, int sampleCount, int maxBounceCount);

    void resize(sf::Vector2i size);

    void renderFrame();

    void saveToFile(const std::string& name);

    int getPixelDifference() const;

    int getMaterialId(const Material& material);

    int addTexture(const std::string& filePath);

    void addFile(const std::string& filePath, sf::Vector3f offset = sf::Vector3f(0, 0, 0), sf::Vector3f scale = sf::Vector3f(1, 1, 1), sf::Vector3f rotation = sf::Vector3f(0, 0, 0));

    void addCornellBox(const Material& up, const Material& down, const Material& left, const Material& right, const Material& forward, const Material& backward, const Material& lightSource);

    void addCornellBox(const Material& up, const Material& down, const Material& left, const Material& right, const Material& forward, const Material& backward);

    void updateTextures();

    void saveScene(const std::string& filePath);

    void loadScene(const std::string& filePath);

protected:
    sf::RenderTexture buffer1;
    sf::RenderTexture buffer2;
    sf::RenderTexture* bufferTargetTexture;
    sf::IntRect subFrame;

    void loadShader();

private:
    static const char ShaderCode[];
    sf::Shader shader;
};

}