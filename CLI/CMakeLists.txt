add_executable(
    ${PROJECT_NAME}CLI
    main.cpp
)

target_include_directories(
    ${PROJECT_NAME}CLI
    PRIVATE ${INCLUDE}
)

target_link_libraries(${PROJECT_NAME}CLI PRIVATE ${PROJECT_NAME})