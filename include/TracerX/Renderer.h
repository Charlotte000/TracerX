#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>
#include <GL/glew.h>
#include <TracerX/Primitives/Box.h>
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
    std::vector<Box> boxes;
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

    void run(int iterationCount, const std::string imagePath);

    int getPixelDifference();

    void add(Sphere sphere, const Material& material);

    void add(Sphere sphere);

    void add(Box box, const Material& material);

    void add(Box box);

    int add(const Material& material);

    void addFile(const std::string filePath, sf::Vector3f offset = sf::Vector3f(0, 0, 0), sf::Vector3f scale = sf::Vector3f(1, 1, 1), sf::Vector3f rotation = sf::Vector3f(0, 0, 0));

    void addCornellBox(const Material up, const Material down, const Material left, const Material right, const Material forward, const Material backward, const Material lightSource);

    void addCornellBox(const Material up, const Material down, const Material left, const Material right, const Material forward, const Material backward);

    void updateMaterials();

    void updateVertices();

    void updateSpheres();

    void updateIndices();

    void updateMeshes();

    void updateBoxes();

protected:
    const std::string path = "../shader.frag";
    sf::RenderTexture buffer1;
    sf::RenderTexture buffer2;
    GLuint materialBuffer;
    GLuint vertexBuffer;
    GLuint sphereBuffer;
    GLuint indexBuffer;
    GLuint meshBuffer;
    GLuint boxBuffer;

    void loadShader();

    void clear();
};

}