#include "TracerX/Scene.h"
#include "TracerX/Application.h"

int main()
{
    Application app;
    app.init(glm::ivec2(900, 900));
    app.run();
    return 0;
}
