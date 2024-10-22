/**
 * @file Shader.cpp
 */
#include "TracerX/core/GL/Shader.h"

#include <string>
#include <stdexcept>

using namespace TracerX::core::GL;

void Shader::init(const std::vector<unsigned char>& shaderBin)
{
    // Create OpenGL shader
    GLuint shaderHandler = this->initShader(shaderBin, GL_COMPUTE_SHADER);

    // Create OpenGL program
    this->handler = this->initProgram(shaderHandler);

    // Clean OpenGL shader
    glDeleteShader(shaderHandler);
}

void Shader::shutdown()
{
    glDeleteProgram(this->handler);
}

void Shader::use()
{
    glUseProgram(this->handler);
}

glm::uvec3 Shader::getGroups(glm::uvec2 size)
{
    return glm::uvec3(glm::ceil(glm::vec3(size, 1) / glm::vec3(Shader::groupSize)));
}

void Shader::dispatchCompute(glm::uvec3 groups)
{
    glDispatchCompute(groups.x, groups.y, groups.z);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void Shader::stopUse()
{
    glUseProgram(0);
}

GLuint Shader::initShader(const std::vector<unsigned char>& bin, GLenum shaderType)
{
    // Create shader
    GLuint handler = glCreateShader(shaderType);
    glShaderBinary(1, &handler, GL_SHADER_BINARY_FORMAT_SPIR_V, bin.data(), (GLsizei)bin.size());
    glSpecializeShader(handler, "main", 0, nullptr, nullptr);

    // Check shader status
    GLint status;
    glGetShaderiv(handler, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE)
    {
        GLint logSize;
        glGetShaderiv(handler, GL_INFO_LOG_LENGTH, &logSize);
        std::string log(logSize, ' ');
        glGetShaderInfoLog(handler, logSize, nullptr, log.data());
        glDeleteShader(handler);
        throw std::runtime_error(log);
    }

    return handler;
}

GLuint Shader::initProgram(GLuint shaderHandler)
{
    // Create program
    GLuint handler = glCreateProgram();
    glAttachShader(handler, shaderHandler);
    glLinkProgram(handler);

    // Check program status
    GLint status;
    glGetProgramiv(handler, GL_LINK_STATUS, &status);
    if (status != GL_TRUE)
    {
        GLint logSize;
        glGetProgramiv(handler, GL_INFO_LOG_LENGTH, &logSize);
        std::string log(logSize, ' ');
        glGetProgramInfoLog(handler, logSize, nullptr, log.data());
        glDeleteShader(shaderHandler);
        glDeleteProgram(handler);
        throw std::runtime_error(log);
    }

    return handler;
}
