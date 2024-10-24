cmake_minimum_required(VERSION 3.10)
project(App)

set(TX_ASSETS_PATH "${CMAKE_CURRENT_SOURCE_DIR}/assets" CACHE PATH "Assets folder" FORCE)

add_executable(
    ${PROJECT_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/UI.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Application.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/imgui.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/imgui_draw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/imgui_tables.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/imgui_widgets.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/imgui_impl_glfw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/imgui_impl_opengl3.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/libs/imguizmo/ImGuizmo.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/libs/tinyfiledialogs/tinyfiledialogs.cpp
)

add_compile_definitions(ASSETS_PATH="${TX_ASSETS_PATH}")
find_package(OpenGL REQUIRED)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs/glfw)
target_include_directories(
    ${PROJECT_NAME}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libs/imguizmo
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libs/tinyfiledialogs
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libs/tinydir
)

if (TX_DENOISE AND WIN32)
    file(GLOB OIDN_DLL "${CMAKE_SOURCE_DIR}/core/libs/oidn/bin/*.dll")
    file(COPY ${OIDN_DLL} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()

target_link_libraries(${PROJECT_NAME} TracerX OpenGL::GL glfw)
