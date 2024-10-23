/**
 * @file Shader.h
 */
#pragma once

#include <string>
#include <vector>
#include <GL/glew.h>
#include <filesystem>
#include <glm/glm.hpp>

namespace TracerX::core::GL
{

/**
 * @brief An OpenGL shader wrapper class.
 * 
 * The shader uses a SPIR-V binary format for loading the shader code if the TX_SPIRV macro is defined.  
 * Otherwise, it uses the GLSL source code provided in the __shaders__ directory.
 * 
 * @remark Should not be used directly and is only used internally by the renderer.
 */
class Shader
{
public:
#if TX_SPIRV
    void init(const std::vector<unsigned char>& shaderBin);
#else
    void init(const std::filesystem::path& shaderSrc);
#endif
    void shutdown();
    void use();

    static glm::uvec3 getGroups(glm::uvec2 size);
    static void dispatchCompute(glm::uvec3 groups);
    static void stopUse();
private:
    GLuint handler;

    static inline const glm::uvec3 groupSize = glm::uvec3(16, 16, 1);

#if TX_SPIRV
    static GLuint initShader(const std::vector<unsigned char>& bin, GLenum shaderType);
#else
    static GLuint initShader(const std::filesystem::path& shaderSrc, GLenum shaderType);
    static std::string loadShader(const std::filesystem::path& path);
#endif
    static GLuint initProgram(GLuint shaderHandler);
    static void checkShader(GLuint shaderHandler);
};

}
