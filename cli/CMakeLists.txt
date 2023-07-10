add_executable(
    ${PROJECT_NAME}CLI
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Application.cpp
)

target_include_directories(
    ${PROJECT_NAME}CLI
    PRIVATE ${CORE_INCLUDE}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME}CLI PRIVATE ${PROJECT_NAME})