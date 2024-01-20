#include <TracerX/Application.h>
#include <TracerX/Camera.h>
#include <SFML/Graphics.hpp>

using namespace TracerX;
using namespace sf;

int main()
{
    Application app;
    app.create(Vector2i(900, 900), Camera(Vector3f(0, 0, -2), Vector3f(0, 0, 1), Vector3f(0, 1, 0), 3, .005f), 5, 5);
    app.subDivisor = Vector2i(2, 2);

    Material red = Material::Matte(Vector3f(1, 0, 0));
    Material green = Material::Matte(Vector3f(0, 1, 0));
    Material white = Material::Matte(Vector3f(1, 1, 1));
    Material lightSource = Material::LightSource(Vector3f(1, 1, 1), 2);
    Material glass = Material::Transparent(Vector3f(1, 1, 1), .5f, Vector3f(1, 1, 1), .4f);
    
    app.addCornellBox(white, white, red, green, white, white, lightSource);
    app.spheres.add(Sphere(Vector3f(0, -.5f, 0), .5f, app.getMaterialId(glass)));

    app.run();
    return 0;
}