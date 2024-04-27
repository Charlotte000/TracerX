cmake_minimum_required(VERSION 3.10)
project(TracerXProject)

option(TX_BUILD_EDITOR "Build graphic editor" ON)

add_subdirectory(core)
if (TX_BUILD_EDITOR)
    add_subdirectory(app)
endif()