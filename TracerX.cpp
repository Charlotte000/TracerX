#include "Renderer.h"
#include "OBJLoader.h"

int main()
{
    Material red(Vector3f(1, 0, 0), 1);
    Material green(Vector3f(0, 1, 0), 1);
    Material white(Vector3f(1, 1, 1), 1);
    Material lightSource = Material::LightSource(Vector3f(1, 1, 1), 2);
    Material glass = Material::Transparent(Vector3f(1, 1, 1), .8f, Vector3f(1, 1, 1), .4f);
    Material dark(Vector3f(.2f, .2f, .2f), .3f);
    Renderer renderer(Vector2i(1000, 1000), Camera(Vector3f(0, 0, -3), Vector3f(0, 0, 1), Vector3f(0, 1, 0), 3, .2f), 5, 5);

    objl::Loader loader;
    loader.LoadFile("C:\\Users\\ramil\\Downloads\\king.obj");

    objl::Mesh mesh = loader.LoadedMeshes[0];

    Vector3f delta(0, -1, 0);
    float angle = 3.1415f * .75f;
    float scale = 0.35f;
    for (int i = 0; i < mesh.Indices.size(); i += 3)
    {
        int i1 = mesh.Indices[i];
        int i2 = mesh.Indices[i + 1];
        int i3 = mesh.Indices[i + 2];
        Vector3f a = scale * rotateY(mesh.Vertices[i1].Position, angle) + delta;
        Vector3f b = scale * rotateY(mesh.Vertices[i2].Position, angle) + delta;
        Vector3f c = scale * rotateY(mesh.Vertices[i3].Position, angle) + delta;
        //renderer.add(Triangle(a, b, c), dark);
    }
    
    renderer.add(AABB(Vector3f(0, -1.5f, 0), Vector3f(10000, 1, 10000)), white);
    //renderer.addCornellBox(white, white, red, green, white, white, lightSource);
    renderer.add(Sphere(Vector3f(0, -.5f, 0), .5f), glass);

    renderer.loadScene();

    renderer.runVisual();
    return 0;
}