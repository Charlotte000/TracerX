cmake_minimum_required(VERSION 3.10)
project(TracerX)

set(
    SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Primitives/Box.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Primitives/Mesh.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Primitives/Sphere.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Animation.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Camera.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Environment.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Material.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Renderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/VectorMath.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Vertex3.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/SSBO.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/UBO.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Application.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/UI.cpp
    
    # ImGui
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/ImGui/imgui_draw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/ImGui/imgui_stdlib.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/ImGui/imgui_UBO.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/ImGui/imgui_tables.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/ImGui/imgui_widgets.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/ImGui/imgui-SFML.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/ImGui/imgui.cpp

    # GLEW
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/GLEW/src/glew.c
)

add_executable(${PROJECT_NAME} ${SRC})

# GLEW
add_compile_definitions(GLEW_STATIC)
target_include_directories(
    ${PROJECT_NAME}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libs/GLEW/include
)

# OpenGL
find_package(OpenGL REQUIRED)

# ImGui
target_include_directories(
    ${PROJECT_NAME}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libs/ImGui
)

# OBJ-Loader
target_include_directories(
    ${PROJECT_NAME}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libs/OBJ-Loader
)

# SFML
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs/SFML)

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/
)

target_link_libraries(${PROJECT_NAME} OpenGL::GL sfml-graphics sfml-window sfml-system)
