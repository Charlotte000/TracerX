#pragma once

#include <GL/glew.h>
#include <SFML/Graphics.hpp>
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
    std::vector<sf::Texture> textures;
    Camera camera;
    sf::Vector2i size;
    Environment environment;
    sf::Shader shader;
    int frameCount = 1;
    int sampleCount = 0;
    int maxBounceCount = 0;
    int subStage = 0;
    sf::Vector2i subDivisor = sf::Vector2i(1, 1);
    sf::RenderWindow window;
    sf::RenderTexture windowBuffer;
    bool isProgressive = false;
    bool isCameraControl = false;
    bool showCursor = true;

    Renderer(sf::Vector2i size, Camera camera, int sampleCount, int maxBounceCount);

    void run();

    void runHeadless(int iterationCount, const std::string imagePath);

    int getPixelDifference() const;

    void add(Sphere sphere, const Material& material);

    void add(Sphere sphere);

    void add(Box box, const Material& material);

    void add(Box box);

    int add(const Material& material);

    int addTexture(const std::string filePath);

    void addFile(const std::string filePath, sf::Vector3f offset = sf::Vector3f(0, 0, 0), sf::Vector3f scale = sf::Vector3f(1, 1, 1), sf::Vector3f rotation = sf::Vector3f(0, 0, 0));

    void addCornellBox(const Material up, const Material down, const Material left, const Material right, const Material forward, const Material backward, const Material lightSource);

    void addCornellBox(const Material up, const Material down, const Material left, const Material right, const Material forward, const Material backward);

    void updateMaterials() const;

    void updateVertices() const;

    void updateSpheres();

    void updateIndices() const;

    void updateMeshes();

    void updateBoxes();

    void updateTextures();

    void reset();

protected:
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

private:
    static const std::string ShaderCode;
    sf::RectangleShape cursor;
};

}