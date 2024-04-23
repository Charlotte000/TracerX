cmake_minimum_required(VERSION 3.10)
project(TracerX)

# SRC
set(
    SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/UI.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Quad.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Scene.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Image.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Shader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Camera.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Buffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Texture.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Material.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Renderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Application.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/FrameBuffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/TextureArray.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/libs/glew/src/glew.c

    ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/imgui.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/imgui_draw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/imgui_tables.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/imgui_widgets.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/imgui_impl_glfw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/imgui_impl_opengl3.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/libs/imguizmo/ImGuizmo.cpp
)

add_executable(
    ${PROJECT_NAME}
    ${SRC}
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# GLEW
add_compile_definitions(GLEW_STATIC)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/glew/include)

# OpenGL
find_package(OpenGL REQUIRED)

# GLFW
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs/glfw)

# ImGui
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui)

# ImGuizmo
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/imguizmo)

# TinyDir
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/tinydir)

# TinyGLTF
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/tinygltf)

# GLM
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs/glm)

# STB
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/stb)

# OIDN
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/oidn/include)
target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libs/oidn/lib)
if(WIN32)
    file(GLOB OIDN_DLL "${CMAKE_CURRENT_SOURCE_DIR}/libs/oidn/bin/*.dll")
    file(COPY ${OIDN_DLL} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    set(OIDN_LIB OpenImageDenoise OpenImageDenoise_core)
elseif(UNIX)
    set(OIDN_LIB OpenImageDenoise)
else()
    message(FATAL_ERROR "Unsupported target system")
endif()

# FastBVH
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs/fastBVH)

target_link_libraries(${PROJECT_NAME} OpenGL::GL glfw glm::glm ${OIDN_LIB} FastBVH)
