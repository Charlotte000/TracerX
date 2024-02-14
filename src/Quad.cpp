#include "TracerX/Quad.h"


bool Quad::initialized = false;
GLuint Quad::handler = 0;

void Quad::draw()
{
    if (!Quad::initialized)
    {
        Quad::init();
    }

    glBindVertexArray(Quad::handler);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void Quad::init()
{
    glGenVertexArrays(1, &Quad::handler);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindVertexArray(Quad::handler);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    float vertices[] =
    {
        -1.0f, 1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f, 1.0f,
        1.0f, -1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));

    glBindVertexArray(0);

    Quad::initialized = true;
}
