cmake_minimum_required(VERSION 3.24.4)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(polyphemus)

include(cmake/CPM.cmake)

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
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_impl_opengl3.cpp
)

set(IMGUI_HEADERS
    ${IMGUI_DIR}/imgui.h
    ${IMGUI_DIR}/imconfig.h
    ${IMGUI_DIR}/imgui_internal.h
    ${IMGUI_DIR}/imgui_impl_opengl3.h
    ${IMGUI_DIR}/imgui_impl_opengl3_loader.h
)
add_library(imgui STATIC ${IMGUI_SOURCES} ${IMGUI_HEADERS})
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

add_executable(polyphemus main.cpp)
target_link_libraries(polyphemus PRIVATE imgui glfw glad OpenGL::GL)

# To make it work on macOS
target_link_libraries(polyphemus PRIVATE "-framework Cocoa" "-framework IOKit" "-framework CoreVideo")

