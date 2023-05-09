#include <TracerX/Renderer.h>
#include <SFML/Graphics.hpp>

using namespace TracerX;
using namespace sf;

int main()
{
    Camera camera(Vector3f(0, 0, -3), Vector3f(0, 0, 1), Vector3f(0, 1, 0), 3, .005f);
    Renderer renderer(Vector2i(900, 900), camera, 5, 5);
    renderer.subDivisor = Vector2i(2, 2);

    Material red(Vector3f(1, 0, 0), 1);
    Material green(Vector3f(0, 1, 0), 1);
    Material white(Vector3f(1, 1, 1), 1);
    Material lightSource = Material::LightSource(Vector3f(1, 1, 1), 2);
    Material glass = Material::Transparent(Vector3f(1, 1, 1), .5f, Vector3f(1, 1, 1), .4f);
    
    renderer.addCornellBox(white, white, red, green, white, white, lightSource);
    renderer.add(Sphere(Vector3f(0, -.5f, 0), .5f), glass);

    renderer.runHeadless(10, "image.png");
    return 0;
}