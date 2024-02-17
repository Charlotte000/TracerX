#include "TracerX/Shader.h"

#include <fstream>


std::string Shader::includeIndentifier = "#include";

void Shader::init(const std::string& vertexPath, const std::string& fragmentPath)
{
    // Create OpenGL shaders
    GLuint vertexHandler = this->initShader(vertexPath, GL_VERTEX_SHADER);
    GLuint fragmentHandler = this->initShader(fragmentPath, GL_FRAGMENT_SHADER);

    // Create OpenGL programs
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

void Shader::updateParam(const std::string& name, int value)
{
    glUniform1i(glGetUniformLocation(this->handler, name.c_str()), value);
}

void Shader::updateParam(const std::string& name, float value)
{
    glUniform1f(glGetUniformLocation(this->handler, name.c_str()), value);
}

void Shader::updateParam(const std::string& name, glm::vec3 value)
{
    glUniform3f(glGetUniformLocation(this->handler, name.c_str()), value.x, value.y, value.z);
}

void Shader::stopUse()
{
    glUseProgram(0);
}

GLuint Shader::initShader(const std::string& main_path, GLenum shaderType)
{
    const std::string src = Shader::load(main_path);
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

const std::string Shader::load(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        throw std::runtime_error("Shader source not found: " + path);
    }

    std::string code = "";

    std::string line;
    while (std::getline(file, line))
    {
        if (line.find(includeIndentifier) != line.npos)
        {
            line.erase(0, Shader::includeIndentifier.size() + 1);
            code += Shader::load(path.substr(0, path.rfind('/') + 1) + line) + '\n';
            continue;
        }

        code += line + '\n';
    }

    file.close();

    return code;
}
