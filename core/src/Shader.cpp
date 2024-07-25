/**
 * @file Shader.cpp
 */
#include "TracerX/Shader.h"

#include <fstream>
#include <glm/gtc/type_ptr.hpp>

using namespace TracerX::core;

void Shader::init(const std::string& shaderSrc)
{
    // Create OpenGL shader
    GLuint shaderHandler = this->initShader(shaderSrc, GL_COMPUTE_SHADER);

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

GLint Shader::getLocation(const std::string& name)
{
    std::map<std::string, GLint>::iterator it = this->locations.find(name);
    if (it != this->locations.end())
    {
        return it->second;
    }

    GLint location = glGetUniformLocation(this->handler, name.c_str());
    this->locations[name] = location;
    return location;
}

GLuint Shader::initShader(const std::string& src, GLenum shaderType)
{
    const GLchar* code = (const GLchar*)src.c_str();
    GLuint handler = glCreateShader(shaderType);
    glShaderSource(handler, 1, &code, 0);
    glCompileShader(handler);

    GLint status = 0;
    glGetShaderiv(handler, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        std::string msg;
        GLint logSize = 0;
        glGetShaderiv(handler, GL_INFO_LOG_LENGTH, &logSize);
        char* info = new char[logSize + 1];
        glGetShaderInfoLog(handler, logSize, NULL, info);
        msg += info;
        delete[] info;
        glDeleteShader(handler);
        throw std::runtime_error(msg);
    }

    return handler;
}

GLuint Shader::initProgram(GLuint shaderHandler)
{
    GLuint handler = glCreateProgram();
    glAttachShader(handler, shaderHandler);
    glLinkProgram(handler);

    GLint success = 0;
    glGetProgramiv(handler, GL_LINK_STATUS, &success);
    if (success != GL_TRUE)
    {
        std::string msg;
        GLint logSize = 0;
        glGetProgramiv(handler, GL_INFO_LOG_LENGTH, &logSize);
        char* info = new char[logSize + 1];
        glGetShaderInfoLog(handler, logSize, NULL, info);
        msg += info;
        delete[] info;
        glDeleteProgram(handler);
        throw std::runtime_error(msg);
    }

    return handler;
}
