/**
 * @file Shader.cpp
 */
#include "TracerX/Shader.h"

#include <fstream>
#include <glm/gtc/type_ptr.hpp>

using namespace TracerX::core;

void Shader::init(const std::string& vertexSrc, const std::string& fragmentSrc)
{
    // Create OpenGL shaders
    GLuint vertexHandler = this->initShader(vertexSrc, GL_VERTEX_SHADER);
    GLuint fragmentHandler = this->initShader(fragmentSrc, GL_FRAGMENT_SHADER);

    // Create OpenGL program
    this->handler = this->initProgram(vertexHandler, fragmentHandler);

    // Clean OpenGL shaders
    glDeleteShader(vertexHandler);
    glDeleteShader(fragmentHandler);
}

void Shader::shutdown()
{
    glDeleteProgram(this->handler);
}

void Shader::use()
{
    glUseProgram(this->handler);
}

void Shader::updateParam(const std::string& name, unsigned int value)
{
    glUniform1ui(glGetUniformLocation(this->handler, name.c_str()), value);
}

void Shader::updateParam(const std::string& name, float value)
{
    glUniform1f(glGetUniformLocation(this->handler, name.c_str()), value);
}

void Shader::updateParam(const std::string& name, glm::vec3 value)
{
    glUniform3f(glGetUniformLocation(this->handler, name.c_str()), value.x, value.y, value.z);
}

void Shader::updateParam(const std::string& name, glm::mat3 value)
{
    glUniformMatrix3fv(glGetUniformLocation(this->handler, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::updateParam(const std::string& name, bool value)
{
    glUniform1i(glGetUniformLocation(this->handler, name.c_str()), value);
}

void Shader::stopUse()
{
    glUseProgram(0);
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

GLuint Shader::initProgram(GLuint vertexHandler, GLuint fragmentHandler)
{
    GLuint handler = glCreateProgram();
    glAttachShader(handler, vertexHandler);
    glAttachShader(handler, fragmentHandler);
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
