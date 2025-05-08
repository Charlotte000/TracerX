cmake_minimum_required(VERSION 3.10)
project(Example)
set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

# TracerX
target_link_libraries(${PROJECT_NAME} PRIVATE TracerX)
if(TX_DENOISE AND WIN32)
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
