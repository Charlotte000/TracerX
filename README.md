# TracerX

Ray tracing app

# Renders


![](img/image0.png)
![](img/image1.png)
![](img/image2.png)

# Features
- Geometries:
    - Sphere
    - Axis-aligned box
    - Triangle
- Configurable ray per frame count
- Simple environment
- Camera's lens distortion
    - Focal length
    - Blur strength
- Progressive rendering
- Different material's properties (for more information visit [PBR materials](https://learn.microsoft.com/en-us/azure/remote-rendering/overview/features/pbr-materials)):
    - Albedo color
    - Roughness
    - Metalness
    - Metalness color
    - Emission color
    - Emission strength
    - Fresnel color
    - Fresnel strength
- Visual mode
- CLI mode

# ToDo
- Refraction
- Texture support

# Visual Mode Control
- WASD - camera left, right, forward, backward movement
- LShif, LCtrl - camera up, down movement
- Mouse - camera rotation
- Q, E - camera tilt
- Enter - start progressive rendering
- R - save current image

# Quickstart
```c++
#include "Renderer.h"

int main()
{
    Material red(Vector3f(1, 0, 0), 1);
    Material green(Vector3f(0, 1, 0), 1);
    Material white(Vector3f(1, 1, 1), 1);
    Material lightSource(Vector3f(0, 0, 0), 0, Vector3f(0, 0, 0), 0, Vector3f(1, 1, 1), 5);
    Material mirror(Vector3f(0, 0, 0), 0, Vector3f(1, 1, 1), 1);

    Renderer renderer(Vector2i(600, 600), Camera(Vector3f(0, 0, -3), Vector3f(0, 0, 1), Vector3f(0, 1, 0), 3, .005f));
    
    renderer.addCornellBox(white, white, red, green, white, white, lightSource);
    renderer.add(Sphere(Vector3f(0, -.5, 0), .5), mirror);
    renderer.loadScene(5, 5, true);

    renderer.runVisual();
    return 0;
}
```

# External libraries used by TracerX
- [SFML/SFML](https://github.com/SFML/SFML)
- [Bly7/OBJ-Loader](https://github.com/Bly7/OBJ-Loader)