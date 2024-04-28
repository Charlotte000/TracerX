cmake_minimum_required(VERSION 3.10)
project(TracerXProject)

option(TX_BUILD_EDITOR "Build graphic editor" ON)
option(TX_DENOISE "Include denoise functionality" ON)
option(TX_BUILD_EXAMPLE "Build example" OFF)

if (TX_DENOISE)
    add_compile_definitions(TX_DENOISE)
endif()

add_subdirectory(core)

if (TX_BUILD_EDITOR)
    add_subdirectory(app)
endif()

if (TX_BUILD_EXAMPLE)
    add_subdirectory(example)
endif()
