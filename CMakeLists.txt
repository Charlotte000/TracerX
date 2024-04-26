cmake_minimum_required(VERSION 3.10)
project(TracerXProject)

option(ONLY_CORE "Build only core library" OFF)

add_subdirectory(core)
if (NOT ONLY_CORE)
    add_subdirectory(app)
endif()