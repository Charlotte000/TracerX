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
 * If the TX_SPIRV macro is defined, this class uses a SPIR-V binary format for loading the shader code.
 * Other wise, it uses GLSL source code.
 * 
 * @remark Should not be used directly and is only used internally by the Renderer.
 */
class Shader
{
public:
    void init(const unsigned char shaderSrc[], const size_t shaderSrcSize);
    void init(const char shaderSrc[]);
    void shutdown();
    void use();
#if !NDEBUG
    void reload(const std::filesystem::path& path);
#endif

    static glm::uvec3 getGroups(glm::uvec2 size);
    static void dispatchCompute(glm::uvec3 groups);
    static void stopUse();
private:
    unsigned int handler;

    static inline const glm::uvec3 groupSize = glm::uvec3(16, 16, 1);

    static unsigned int initShader(const unsigned char shaderSrc[], const size_t shaderSrcSize, unsigned int shaderType);
    static unsigned int initShader(const char shaderSrc[], unsigned int shaderType);
    static unsigned int initProgram(unsigned int shaderHandler);
#if !NDEBUG
    static std::string loadShader(const std::filesystem::path& path);
#endif
    static void checkShader(unsigned int shaderHandler);
};

}
