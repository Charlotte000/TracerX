cmake_minimum_required(VERSION 3.10)
project(TracerX)

set(src ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(imgui ${CMAKE_CURRENT_SOURCE_DIR}/libs/ImGui)

add_executable(
    ${PROJECT_NAME}
    ${src}/Primitives/Box.cpp
    ${src}/Primitives/Mesh.cpp
    ${src}/Primitives/Sphere.cpp
    ${src}/Camera.cpp
    ${src}/Environment.cpp
    ${src}/main.cpp
    ${src}/Material.cpp
    ${src}/Renderer.cpp
    ${src}/RendererUI.cpp
    ${src}/RendererVisual.cpp
    ${src}/VectorMath.cpp
    ${src}/Vertex3.cpp

    ${imgui}/imgui_draw.cpp
    ${imgui}/imgui_stdlib.cpp
    ${imgui}/imgui_tables.cpp
    ${imgui}/imgui_widgets.cpp
    ${imgui}/imgui-SFML.cpp
    ${imgui}/imgui.cpp
)

add_compile_definitions(GLEW_STATIC)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/shader.frag DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-window sfml-system GLEW GL)
target_include_directories(
    ${PROJECT_NAME}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libs
)