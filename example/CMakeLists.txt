cmake_minimum_required(VERSION 3.10)
project(Example)
set(CMAKE_CXX_STANDARD 20)

add_executable(
    ${PROJECT_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} TracerX OpenGL::GL glfw)

if(WIN32)
    file(GLOB OIDN_DLL "${CMAKE_SOURCE_DIR}/core/libs/oidn/bin/*.dll")
    file(COPY ${OIDN_DLL} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()
