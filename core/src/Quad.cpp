/**
 * @file Quad.cpp
 */
#include "TracerX/Quad.h"

using namespace TracerX::core;

void Quad::init()
{
    glGenVertexArrays(1, &this->handler);

    glGenBuffers(1, &this->vertexHandler);
    glBindVertexArray(this->handler);
    glBindBuffer(GL_ARRAY_BUFFER, this->vertexHandler);

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
}

void Quad::draw()
{
    glBindVertexArray(this->handler);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void Quad::shutdown()
{
    glDeleteVertexArrays(1, &this->handler);
    glDeleteBuffers(1, &this->vertexHandler);
}
