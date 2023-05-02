#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>
#include <GL/glew.h>
#include <TracerX/Primitives/AABB.h>
#include <TracerX/Primitives/Sphere.h>
#include <TracerX/Camera.h>
#include <TracerX/Environment.h>
#include <TracerX/Vertex3.h>
#include <TracerX/Primitives/Mesh.h>
#include <TracerX/Material.h>

namespace TracerX
{

class Renderer
{
public:
    std::vector<Material> materials;
    std::vector<Sphere> spheres;
    std::vector<AABB> aabbs;
    std::vector<int> indices;
    std::vector<Vertex3> vertices;
    std::vector<Mesh> meshes;
    std::vector<sf::Texture> albedoMaps;
    Camera camera;
    sf::Vector2i size;
    Environment environment;
    sf::Shader shader;
    int frameCount = 1;
    int subStage = 0;
    int sampleCount = 0;
    int maxBounceCount = 0;
    sf::Vector2i subDivisor = sf::Vector2i(1, 1);

    Renderer(sf::Vector2i size, Camera camera, int sampleCount, int maxBounceCount);

    void loadScene();

    void run(int iterationCount, const std::string imagePath);

    int getPixelDifference();

    void add(Sphere sphere, const Material material);

    void add(Sphere sphere);

    void add(AABB aabb, const Material material);

    void add(AABB aabb);

    int add(const Material& material);

    void addFile(const std::string filePath, sf::Vector3f offset = sf::Vector3f(0, 0, 0), sf::Vector3f scale = sf::Vector3f(1, 1, 1), sf::Vector3f rotation = sf::Vector3f(0, 0, 0));

    void addCornellBox(const Material up, const Material down, const Material left, const Material right, const Material forward, const Material backward, const Material lightSource);

    void addCornellBox(const Material up, const Material down, const Material left, const Material right, const Material forward, const Material backward);

    void setMaterials();

    void setVertices();

    void setSpheres();

    void setIndices();

    void setMeshes();

    void setAABBs();

protected:
    const std::string path = "../shader.frag";
    sf::RenderTexture buffer1;
    sf::RenderTexture buffer2;
    GLuint materialBuffer;
    GLuint vertexBuffer;
    GLuint sphereBuffer;
    GLuint indexBuffer;
    GLuint meshBuffer;
    GLuint aabbBuffer;

    void loadShader();

    void clear();
};

}