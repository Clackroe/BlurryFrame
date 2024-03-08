
cmake_minimum_required(VERSION 3.5)
project(BlurFrame)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)

# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Include directories
include_directories(include)

# GLFW
add_subdirectory(libs/glfw)
include_directories(libs/glfw/include)

# Glad
add_library(glad STATIC libs/glad/src/glad.c)
include_directories(libs/glad/include)

# GLM
include_directories(libs/glm)

# ImGui
set(IMGUI_PATH  "libs/imgui/")

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
file(GLOB IMGUI_SOURCES
        "libs/imgui/*.h"
        "libs/imgui/*.cpp"
        "libs/imgui/backends/imgui_impl_glfw.h"
        "libs/imgui/backends/imgui_impl_opengl3.h"
        "libs/imgui/backends/imgui_impl_glfw.cpp"
        "libs/imgui/backends/imgui_impl_opengl3.cpp")

add_library(imgui ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC ${IMGUI_PATH} libs/imgui/backends)


# add_library(imgui STATIC libs/imgui/imgui.cpp libs/imgui/imgui_demo.cpp libs/imgui/imgui_draw.cpp libs/imgui/imgui_widgets.cpp)
include_directories(libs/imgui)

# Executable
add_executable(Blur ${SOURCES})
target_link_libraries(Blur glfw glad imgui)
