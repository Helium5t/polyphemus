cmake_minimum_required(VERSION 3.24.4)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(polyphemus)

# DO NOT COMMIT
add_compile_options(-Wno-deprecated-declarations)

include(cmake/CPM.cmake)

####### DEPENDENCIES #######
# OpenGL
find_package(OpenGL)

# GLFW
# CPMAddPackage("gh:glfw/glfw#3.3.8")
CPMAddPackage(
    NAME glfw
    GITHUB_REPOSITORY glfw/glfw
    GIT_TAG 3.3.8
    OPTIONS
        "GLFW_BUILD_EXAMPLES OFF"
        "GLFW_BUILD_TESTS OFF"
        "GLFW_BUILD_DOCS OFF"
        "GLFW_INSTALL OFF"
        "BUILD_SHARED_LIBS OFF"
)

# IMGUI (DearImGui so just adding sources locally)
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps/ImGui)
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_impl_opengl3.cpp
    ${IMGUI_DIR}/imgui_impl_glfw.cpp
)

set(IMGUI_HEADERS
    ${IMGUI_DIR}/imgui.h
    ${IMGUI_DIR}/imconfig.h
    ${IMGUI_DIR}/imgui_internal.h
    ${IMGUI_DIR}/imgui_impl_opengl3.h
    ${IMGUI_DIR}/imgui_impl_opengl3_loader.h
    ${IMGUI_DIR}/imgui_impl_glfw.h
)
add_library(imgui STATIC ${IMGUI_SOURCES} ${IMGUI_HEADERS})
target_link_libraries(imgui PRIVATE glfw)
target_include_directories(imgui PUBLIC ${IMGUI_DIR})


# GLAD
set(GLAD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps/GLAD/include)
set(
    GLAD_SOURCES
    deps/GLAD/include/Source/glad.c
)

set(
    GLAD_HEADERS
    deps/GLAD/include/glad/glad.h
)
add_library(glad STATIC ${GLAD_SOURCES} ${GLAD_HEADERS})
target_include_directories(glad PUBLIC ${GLAD_DIR})

# JSON Parser

CPMAddPackage("gh:nlohmann/json#v3.11.2")

# GLTF Parser (Deprecated in favor of asset importer)

# CPMAddPackage(
#     NAME tinygltf
#     GITHUB_REPOSITORY syoyo/tinygltf
#     GIT_TAG v2.8.0
# )

# Asset Importer 
CPMAddPackage(
    NAME assimp
    GITHUB_REPOSITORY assimp/assimp
    GIT_TAG v5.2.5
)

# GLM 
CPMAddPackage("gh:g-truc/glm#0.9.9.8")

# STB IMAGE

set(STB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps/stb_image)
set(
    STB_HEADERS
    ${STB_DIR}/stb_image_write.h
    ${STB_DIR}/stb_image.h
)
add_library(stb_image INTERFACE)          
target_include_directories(stb_image
    INTERFACE
    ${STB_DIR}
)

####### END DEPENDENCIES #######
add_executable(
    polyphemus 
    main.cpp
    model.cpp
    mesh.cpp
    textures.cpp
    render.cpp
    shaders.cpp
    camera.cpp
    transform.cpp
    
    scene.cpp
    chessScene.cpp
    genericModelScene.cpp
    frostbiteModelScene.cpp
    customTextureScene.cpp
    scenePicker.cpp)

target_link_libraries(polyphemus PRIVATE stb_image imgui glfw glad nlohmann_json::nlohmann_json OpenGL::GL glm assimp)

# To make it work on macOS
target_link_libraries(polyphemus PRIVATE "-framework Cocoa" "-framework IOKit" "-framework CoreVideo")

