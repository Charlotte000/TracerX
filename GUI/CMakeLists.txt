add_executable(
    ${PROJECT_NAME}GUI
    main.cpp
)

target_include_directories(
    ${PROJECT_NAME}GUI
    PRIVATE ${INCLUDE}
)

target_link_libraries(${PROJECT_NAME}GUI PRIVATE ${PROJECT_NAME})