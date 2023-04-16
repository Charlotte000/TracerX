#include "Renderer.h"

int main()
{
    Material red(Vector3f(1, 0, 0), 1);
    Material green(Vector3f(0, 1, 0), 1);
    Material blue(Vector3f(0, 0, 1), 1);
    Material white(Vector3f(1, 1, 1), 1);
    Material lightSource(Vector3f(0, 0, 0), 0, Vector3f(0, 0, 0), 0, Vector3f(1, 1, 1), 1);
    Material mirror(Vector3f(0, 0, 0), 0, Vector3f(1, 1, 1), 1);

    Renderer renderer(Vector2i(600, 600), Camera(Vector3f(0, 0, -3), Vector3f(0, 0, 1), Vector3f(0, 1, 0), 3, .005f));
    
    renderer.addCornellBox(white, white, red, green, white, white, lightSource);
    renderer.add(Sphere(Vector3f(0, -.5, 0), .5), mirror);

    renderer.loadScene(5, 5, true);

    renderer.runVisual();
    return 0;
}