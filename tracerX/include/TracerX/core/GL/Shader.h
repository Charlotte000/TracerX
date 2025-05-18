/**
 * @file Shader.h
 */
#pragma once

#include <string>
#include <vector>
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
    void init(const unsigned char shaderSrc[], const size_t shaderSrcSize);
#elif NDEBUG
    void init(const char shaderSrc[]);
#else
    void init(const std::filesystem::path& shaderSrc);
#endif
    void shutdown();
    void use();

    static glm::uvec3 getGroups(glm::uvec2 size);
    static void dispatchCompute(glm::uvec3 groups);
    static void stopUse();
private:
    unsigned int handler;

    static inline const glm::uvec3 groupSize = glm::uvec3(16, 16, 1);

#if TX_SPIRV
    static unsigned int initShader(const unsigned char shaderSrc[], const size_t shaderSrcSize, unsigned int shaderType);
#else
    static unsigned int initShader(const char shaderSrc[], unsigned int shaderType);
#endif
    static unsigned int initProgram(unsigned int shaderHandler);
#if !TX_SPIRV && !NDEBUG
    static std::string loadShader(const std::filesystem::path& path);
#endif
    static void checkShader(unsigned int shaderHandler);
};

}
