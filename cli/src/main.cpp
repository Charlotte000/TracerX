#include "CLI/Application.h"
#include <TracerX/Camera.h>
#include <SFML/Graphics.hpp>

using namespace CLI;
using namespace TracerX;
using namespace sf;

int main()
{
    Application app;
    app.create(Vector2i(900, 900), Camera(Vector3f(0, 0, -2), Vector3f(0, 0, 1), Vector3f(0, 1, 0), 3, .005f), 5, 5);
    app.subDivisor = Vector2i(10, 10);

    Material red(Vector3f(1, 0, 0), 1);
    Material green(Vector3f(0, 1, 0), 1);
    Material white(Vector3f(1, 1, 1), 1);
    Material lightSource = Material::LightSource(Vector3f(1, 1, 1), 2);
    Material glass = Material::Transparent(Vector3f(1, 1, 1), .5f, Vector3f(1, 1, 1), .4f);

    app.addCornellBox(white, white, red, green, white, white, lightSource);
    app.add(Sphere(Vector3f(0, -.5f, 0), .5f), glass);

    app.run(1, "image.png");
    return 0;
}