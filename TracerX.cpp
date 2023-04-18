#include "Renderer.h"

int main()
{
    Material red(Vector3f(1, 0, 0), 1);
    Material green(Vector3f(0, 1, 0), 1);
    Material blue(Vector3f(0, 0, 1), .4f, Vector3f(1, 1, 1), .8f);
    Material white(Vector3f(1, 1, 1), 1);
    Material lightSource = Material::LightSource(Vector3f(1, 1, 1), 2);
    Material mirror(Vector3f(0, 0, 0), 0, Vector3f(1, 1, 1), 1);

    Renderer renderer(Vector2i(600, 600), Camera(Vector3f(0, 0, -3), Vector3f(0, 0, 1), Vector3f(0, 1, 0), 3, .005f));
    
    renderer.addCornellBox(white, white, red, green, white, white, lightSource);
    renderer.add(Sphere(Vector3f(0, -.5f, 0), .5f), Material::Transparent(Vector3f(1, 1, 1), .5f, Vector3f(1, 1, 1), .4f));

    renderer.loadScene(5, 5);

    renderer.runVisual();
    return 0;
}