set(
    IMGUI
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/ImGui/imgui_draw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/ImGui/imgui_stdlib.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/ImGui/imgui_tables.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/ImGui/imgui_widgets.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/ImGui/imgui-SFML.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/ImGui/imgui.cpp
)

add_executable(
    ${PROJECT_NAME}GUI
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Application.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/UI.cpp
    ${IMGUI}
)

target_include_directories(
    ${PROJECT_NAME}GUI
    PRIVATE ${CORE_INCLUDE}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libs
)

target_link_libraries(${PROJECT_NAME}GUI PRIVATE ${PROJECT_NAME})