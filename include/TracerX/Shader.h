#pragma once

#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>


class Shader
{
public:
    static std::string includeIndentifier;

    void init(const std::string& vertexPath, const std::string& fragmentPath);
    void shutdown();
    void use();
    void updateParam(const std::string& name, int value);
    void updateParam(const std::string& name, float value);
    void updateParam(const std::string& name, glm::vec3 value);
    void updateParam(const std::string& name, glm::mat3 value);

    static void stopUse();
private:
    GLuint handler;

    static const std::string load(const std::string& path);
    static GLuint initShader(const std::string& path, GLenum shaderType);
    static GLuint initProgram(GLuint vertexHandler, GLuint fragmentHandler);
};
