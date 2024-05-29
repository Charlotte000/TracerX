cmake_minimum_required(VERSION 3.10)
project(TracerX)

add_library(
    ${PROJECT_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Quad.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Scene.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Image.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Shader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Camera.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Texture.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Material.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Renderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Environment.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/FrameBuffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/TextureArray.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/RendererShaderSrc.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/libs/glew/src/glew.c
)

add_compile_definitions(GLEW_STATIC)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/libs/glew/include
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/libs/tinydir
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/libs/tinygltf
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/libs/stb
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/libs/oidn/include
)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs/glm)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs/fastBVH)

if (TX_DENOISE)
    target_link_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/libs/oidn/lib)
    if(WIN32)
        set(OIDN_LIB OpenImageDenoise OpenImageDenoise_core)
    elseif(UNIX)
        set(OIDN_LIB OpenImageDenoise)
    else()
        message(FATAL_ERROR "Unsupported target system")
    endif()
endif()

target_link_libraries(${PROJECT_NAME} glm::glm ${OIDN_LIB} FastBVH)
