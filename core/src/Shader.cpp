/**
 * @file Shader.cpp
 */
#include "TracerX/core/GL/Shader.h"

#include <array>
#include <fstream>
#include <stdexcept>
#include <GL/glew.h>

using namespace TracerX::core::GL;

#if TX_SPIRV
void Shader::init(const unsigned char shaderBin[], const size_t shaderBinSize)
{
    // Create OpenGL shader
const GLuint shaderHandler = this->initShader(shaderBin, shaderBinSize, GL_COMPUTE_SHADER);

    // Create OpenGL program
    this->handler = this->initProgram(shaderHandler);

    // Clean OpenGL shader
    glDeleteShader(shaderHandler);
}

GLuint Shader::initShader(const unsigned char bin[], const size_t shaderBinSize, unsigned int shaderType)
{
    // Create shader
    const GLuint handler = glCreateShader(shaderType);
    glShaderBinary(1, &handler, GL_SHADER_BINARY_FORMAT_SPIR_V, bin, (GLsizei)shaderBinSize);
    glSpecializeShader(handler, "main", 0, nullptr, nullptr);

    Shader::checkShader(handler);
    return handler;
}
#else
void Shader::init(const std::filesystem::path& shaderSrc)
{
    // Create OpenGL shader
    const GLuint shaderHandler = this->initShader(shaderSrc, GL_COMPUTE_SHADER);

    // Create OpenGL program
    this->handler = this->initProgram(shaderHandler);

    // Clean OpenGL shader
    glDeleteShader(shaderHandler);
}

unsigned int Shader::initShader(const std::filesystem::path& shaderSrc, unsigned int shaderType)
{
    // Create shader
    const std::string src = Shader::loadShader(shaderSrc);
    const GLchar* code = (const GLchar*)src.c_str();
    const GLuint handler = glCreateShader(shaderType);
    glShaderSource(handler, 1, &code, 0);
    glCompileShader(handler);

    Shader::checkShader(handler);
    return handler;
}

std::string Shader::loadShader(const std::filesystem::path& path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        throw std::runtime_error("Shader source not found: " + path.string());
    }

    const std::string includeIdentifier = "#include";

    std::string code;
    std::string line;
    while (std::getline(file, line))
    {
        if (line.find(includeIdentifier) != line.npos)
        {
            // Get include path (remove #include and quotes)
            line.erase(0, includeIdentifier.size() + 2);
            line.pop_back();

            code += Shader::loadShader(path.parent_path() / line) + '\n';
            continue;
        }

        code += line + '\n';
    }

    return code;
}
#endif

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

unsigned int Shader::initProgram(unsigned int shaderHandler)
{
    // Create program
    const GLuint handler = glCreateProgram();
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

void Shader::checkShader(unsigned int shaderHandler)
{
    GLint status;
    glGetShaderiv(shaderHandler, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE)
    {
        GLint logSize;
        glGetShaderiv(shaderHandler, GL_INFO_LOG_LENGTH, &logSize);
        std::string log(logSize, ' ');
        glGetShaderInfoLog(shaderHandler, logSize, nullptr, log.data());
        glDeleteShader(shaderHandler);
        throw std::runtime_error(log);
    }
}
