cmake_minimum_required(VERSION 3.10)
project(App)

cmake_policy(SET CMP0135 NEW)
include(FetchContent)
set(CMAKE_CXX_STANDARD 20)

add_executable(
    ${PROJECT_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Application.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ApplicationUI.cpp
)

# TracerX
target_link_libraries(${PROJECT_NAME} PRIVATE TracerX)
if (TX_DENOISE AND WIN32)
    file(GLOB OIDN_DLL "${OIDN_BIN}/*.dll")
    file(COPY ${OIDN_DLL} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()

# OpenGL
find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)

# GLFW 3.4
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build the GLFW documentation")
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG        3.4
)
FetchContent_MakeAvailable(glfw)
target_include_directories(${PROJECT_NAME} PRIVATE ${glfw_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

# ImGui v1.90.4
FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG        v1.90.4
)
FetchContent_MakeAvailable(imgui)
target_include_directories(
    ${PROJECT_NAME}
    PRIVATE ${imgui_SOURCE_DIR}
    PRIVATE ${imgui_SOURCE_DIR}/backends
)
target_sources(
    ${PROJECT_NAME}
    PRIVATE ${imgui_SOURCE_DIR}/imgui.cpp
    PRIVATE ${imgui_SOURCE_DIR}/imgui_draw.cpp
    PRIVATE ${imgui_SOURCE_DIR}/imgui_tables.cpp
    PRIVATE ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    PRIVATE ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    PRIVATE ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

# ImGuizmo 1.83
FetchContent_Declare(
  imguizmo
  GIT_REPOSITORY https://github.com/CedricGuillemet/ImGuizmo.git
  GIT_TAG        1.83
)
FetchContent_MakeAvailable(imguizmo)
target_include_directories(${PROJECT_NAME} PRIVATE ${imguizmo_SOURCE_DIR})
target_sources(${PROJECT_NAME} PRIVATE ${imguizmo_SOURCE_DIR}/ImGuizmo.cpp)
target_compile_definitions(${PROJECT_NAME} PRIVATE -DIMGUI_DEFINE_MATH_OPERATORS)

# TinyFileDialogs v3.18.2
FetchContent_Declare(
  tinyfiledialogs
  URL      https://sourceforge.net/code-snapshots/git/t/ti/tinyfiledialogs/code.git/tinyfiledialogs-code-29c1b354d75825209adf8cc1979c425885a64d32.zip
  URL_HASH SHA256=381c68cd5330e58d9180807bf82570988aa96c2fb133b4d0d5e8a588af12762f
)
FetchContent_MakeAvailable(tinyfiledialogs)
target_include_directories(${PROJECT_NAME} PRIVATE ${tinyfiledialogs_SOURCE_DIR})
target_sources(${PROJECT_NAME} PRIVATE ${tinyfiledialogs_SOURCE_DIR}/tinyfiledialogs.c)
