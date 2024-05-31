#pragma once

#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

namespace TracerX::core
{

class Shader
{
public:
    void init(const std::string& vertexSrc, const std::string& fragmentSrc);
    void shutdown();
    void use();
    void updateParam(const std::string& name, unsigned int value);
    void updateParam(const std::string& name, float value);
    void updateParam(const std::string& name, glm::vec3 value);
    void updateParam(const std::string& name, glm::mat3 value);
    void updateParam(const std::string& name, bool value);

    static void stopUse();
private:
    GLuint handler;

    static GLuint initShader(const std::string& src, GLenum shaderType);
    static GLuint initProgram(GLuint vertexHandler, GLuint fragmentHandler);
};

}
