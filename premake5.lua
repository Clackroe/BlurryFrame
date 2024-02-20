require "export-compile-commands"
workspace "BlurryFrame"
    configurations { "Debug", "Release" }
    startproject "Blurry"

    flags { "MultiProcessorCompile" }

    filter "configurations:Debug"
        defines { "DEBUG", "DEBUG_SHADER" }
        symbols "On"

    filter "configurations:Release"
        defines { "RELEASE" }
        optimize "Speed"
        flags { "LinkTimeOptimization" }

project "Blurry"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
	architecture "x86_64"

    targetdir "bin/%{cfg.buildcfg}"
    objdir "obj/%{cfg.buildcfg}"

    includedirs { "include/", "libs/glad/include/", "libs/glfw/include/", "libs/glm/"}
    
    files {
        "src/*.cpp",
        "libs/glfw/src/cocoa_init.m",
        "libs/glfw/src/cocoa_joystick.m",
        "libs/glfw/src/cocoa_monitor.m",
        "libs/glfw/src/cocoa_window.m",
        "libs/glfw/src/cocoa_time.c",
        "libs/glfw/src/posix_thread.c",
        "libs/glfw/src/nsgl_context.m",
        "libs/glfw/src/egl_context.c",
        "libs/glfw/src/osmesa_context.c",
        "libs/glfw/src/posix_module.c"
    }

    links { "GLFW", "GLM", "GLAD"}

    filter "system:linux"
        links { "dl", "pthread" }

        defines { "_X11" }

    filter "system:windows"
        defines { "_WINDOWS" }

include "libs/glfw.lua"
include "libs/glad.lua"
include "libs/glm.lua"
-- include "libs/imgui.lua"
