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
void Shader::init(const unsigned char shaderSrc[], const size_t shaderSrcSize)
#elif NDEBUG
void Shader::init(const char shaderSrc[])
#else
void Shader::init(const std::filesystem::path& shaderSrc)
#endif
{
    // Create OpenGL shader
#if TX_SPIRV
    const GLuint shaderHandler = this->initShader(shaderSrc, shaderSrcSize, GL_COMPUTE_SHADER);
#elif NDEBUG
    const GLuint shaderHandler = this->initShader(shaderSrc, GL_COMPUTE_SHADER);
#else
    const GLuint shaderHandler = this->initShader(Shader::loadShader(shaderSrc).c_str(), GL_COMPUTE_SHADER);
#endif

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

#if TX_SPIRV
GLuint Shader::initShader(const unsigned char shaderSrc[], const size_t shaderSrcSize, unsigned int shaderType)
{
    // Create shader
    const GLuint handler = glCreateShader(shaderType);
    glShaderBinary(1, &handler, GL_SHADER_BINARY_FORMAT_SPIR_V, shaderSrc, (GLsizei)shaderSrcSize);
    glSpecializeShader(handler, "main", 0, nullptr, nullptr);

    Shader::checkShader(handler);
    return handler;
}
#else
unsigned int Shader::initShader(const char shaderSrc[], unsigned int shaderType)
{
    // Create shader
    const GLuint handler = glCreateShader(shaderType);
    glShaderSource(handler, 1, &shaderSrc, nullptr);
    glCompileShader(handler);

    Shader::checkShader(handler);
    return handler;
}
#endif

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

#if !TX_SPIRV && !NDEBUG
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
