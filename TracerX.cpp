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

    Renderer renderer(Vector2i(1000, 1000), Camera(Vector3f(0, 0, -3), Vector3f(0, 0, 1), Vector3f(0, 1, 0), 3, .02f), 5, 5);
    
    renderer.addCornellBox(white, white, red, green, white, white, lightSource);
    renderer.add(Sphere(Vector3f(0, -.5f, 0), .5f), glass);

    renderer.loadScene();

    renderer.runVisual();
    return 0;
}