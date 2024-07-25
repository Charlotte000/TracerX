/**
 * @file Shader.h
 */
#pragma once

#include <map>
#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

namespace TracerX::core
{

class Shader
{
public:
    void init(const std::string& shaderSrc);
    void shutdown();
    void use();

    static glm::uvec3 getGroups(glm::uvec2 size);
    static void dispatchCompute(glm::uvec3 groups);
    static void stopUse();
private:
    GLuint handler;
    std::map<std::string, GLint> locations;

    static inline const glm::uvec3 groupSize = glm::uvec3(16, 16, 1);

    GLint getLocation(const std::string& name);

    static GLuint initShader(const std::string& src, GLenum shaderType);
    static GLuint initProgram(GLuint shaderHandler);
};

}
