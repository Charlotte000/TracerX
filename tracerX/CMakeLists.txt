cmake_minimum_required(VERSION 3.10)
project(TracerX)

include(FetchContent)
set(CMAKE_CXX_STANDARD 17)

add_library(
    ${PROJECT_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Scene.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Camera.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Renderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/GLTFLoader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Environment.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/RendererShaderSrc.cpp
)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# FastBVH
set(FASTBVH_BUILD_EXAMPLES OFF CACHE BOOL "Whether or not to build the example programs.")
set(FASTBVH_BUILD_TESTS OFF CACHE BOOL "Whether or not to build the unit tests.")
FetchContent_Declare(
    fastbvh
    GIT_REPOSITORY https://github.com/brandonpelfrey/Fast-BVH.git
    GIT_TAG        f2750b2d3543248b5b29dd97a630d35be5366678
)
FetchContent_MakeAvailable(fastbvh)
target_include_directories(${PROJECT_NAME} PRIVATE ${fastbvh_SOURCE_DIR}/include)

# GLEW 2.2.0
FetchContent_Declare(
    glew
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    URL      https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0.zip
    URL_HASH SHA256=a9046a913774395a095edcc0b0ac2d81c3aacca61787b39839b941e9be14e0d4
)
FetchContent_MakeAvailable(glew)
target_include_directories(${PROJECT_NAME} PUBLIC ${glew_SOURCE_DIR}/include)
target_sources(${PROJECT_NAME} PUBLIC ${glew_SOURCE_DIR}/src/glew.c)
target_compile_definitions(${PROJECT_NAME} PUBLIC GLEW_STATIC)

# GLM v1.0.1
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.1
)
FetchContent_MakeAvailable(glm)
target_include_directories(${PROJECT_NAME} PUBLIC ${glm_SOURCE_DIR})
target_compile_definitions(${PROJECT_NAME} PUBLIC GLM_ENABLE_EXPERIMENTAL)

# OIDN v2.3.2
if (TX_DENOISE)
    if (WIN32)
        set(OIDN_URL https://github.com/RenderKit/oidn/releases/download/v2.3.2/oidn-2.3.2.x64.windows.zip)
        set(OIDN_URL_HASH c4f527b133d955fa5775af64f09e7a03a098f3aaef6b05b1702c1482c5d94fef)
    elseif (UNIX)
        set(OIDN_URL https://github.com/RenderKit/oidn/releases/download/v2.3.2/oidn-2.3.2.x86_64.linux.tar.gz)
        set(OIDN_URL_HASH 4113f7aaac7d04a69162bf1efbd565f561a12e45423c367bbae350e7854034d1)
    else()
        message(FATAL_ERROR "Unsupported target system")
    endif()

    FetchContent_Declare(
        oidn
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        URL      ${OIDN_URL}
        URL_HASH SHA256=${OIDN_URL_HASH}
    )
    FetchContent_MakeAvailable(oidn)
    target_include_directories(${PROJECT_NAME} PRIVATE ${oidn_SOURCE_DIR}/include)
    target_link_directories(${PROJECT_NAME} PUBLIC ${oidn_SOURCE_DIR}/lib)
    target_link_libraries(${PROJECT_NAME} PRIVATE OpenImageDenoise)

    if (WIN32)
        file(GLOB OIDN_DLL "${oidn_SOURCE_DIR}/bin/*.dll")
        set(TX_DENOISE_DLL ${OIDN_DLL} PARENT_SCOPE)
    endif()
endif()

# STB
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG        5c205738c191bcb0abc65c4febfa9bd25ff35234
)
FetchContent_MakeAvailable(stb)
target_include_directories(${PROJECT_NAME} PRIVATE ${stb_SOURCE_DIR})

# TinyGLTF v2.9.4
set(TINYGLTF_BUILD_LOADER_EXAMPLE OFF CACHE BOOL "Build loader_example(load glTF and dump infos)")
FetchContent_Declare(
    tinygltf
    GIT_REPOSITORY https://github.com/syoyo/tinygltf.git
    GIT_TAG        v2.9.4
)
FetchContent_MakeAvailable(tinygltf)
target_include_directories(${PROJECT_NAME} PRIVATE ${tinygltf_SOURCE_DIR})

# OGL
FetchContent_Declare(
    ogl
    GIT_REPOSITORY https://github.com/Charlotte000/OGL.git
    GIT_TAG        main
)
FetchContent_MakeAvailable(ogl)
target_link_libraries(${PROJECT_NAME} PUBLIC OGL)
