cmake_minimum_required(VERSION 3.10)
project(TracerX)

set(CORE_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/core/include)

add_subdirectory(core)
add_subdirectory(gui)
add_subdirectory(cli)