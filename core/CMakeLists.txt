set(
    SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Primitives/Box.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Primitives/Mesh.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Primitives/Sphere.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Camera.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Environment.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Material.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Renderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/VectorMath.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Vertex3.cpp
)

add_library(${PROJECT_NAME} ${SRC})

add_compile_definitions(GLEW_STATIC)

target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-window sfml-system GLEW GL)
target_include_directories(
    ${PROJECT_NAME}
    PRIVATE ${CORE_INCLUDE}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libs
)