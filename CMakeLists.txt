cmake_minimum_required(VERSION 3.10)
project(TracerX)

set(
    SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Primitives/Box.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Primitives/Mesh.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Primitives/Sphere.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Camera.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Environment.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Material.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Renderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/RendererUI.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/VectorMath.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Vertex3.cpp
)

set(
    IMGUI
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/ImGui/imgui_draw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/ImGui/imgui_stdlib.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/ImGui/imgui_tables.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/ImGui/imgui_widgets.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/ImGui/imgui-SFML.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/ImGui/imgui.cpp
)

set(INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(
    ${PROJECT_NAME}
    ${SRC}
    ${IMGUI}
)

add_subdirectory(GUI)
add_subdirectory(CLI)

add_compile_definitions(GLEW_STATIC)

target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-window sfml-system GLEW GL)
target_include_directories(
    ${PROJECT_NAME}
    PRIVATE ${INCLUDE}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libs
)