#include "Application.h"

int main()
{
    Application app;
    app.init(glm::uvec2(900, 900));
    app.run();
    return 0;
}
