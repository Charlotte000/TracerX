cmake_minimum_required(VERSION 3.10)
project(Example)
set(CMAKE_CXX_STANDARD 20)

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

# GLFW
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
