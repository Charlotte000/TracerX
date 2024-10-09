/**
 * @file Shader.h
 */
#pragma once

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>

namespace TracerX::core::GL
{

/**
 * @brief An OpenGL shader wrapper class.
 * 
 * The shader uses a SPIR-V binary format for loading the shader code.
 * 
 * @remark Should not be used directly and is only used internally by the renderer.
 */
class Shader
{
public:
    void init(const std::vector<unsigned char>& shaderBin);
    void shutdown();
    void use();

    static glm::uvec3 getGroups(glm::uvec2 size);
    static void dispatchCompute(glm::uvec3 groups);
    static void stopUse();
private:
    GLuint handler;

    static inline const glm::uvec3 groupSize = glm::uvec3(16, 16, 1);

    static GLuint initShader(const std::vector<unsigned char>& bin, GLenum shaderType);
    static GLuint initProgram(GLuint shaderHandler);
};

}
