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
        "src/*.cpp"
    }

    filter "system:linux"
        links { "GLFW", "GLAD", "GLM", "dl", "pthread" }
        defines { "_X11" }
        include "libs/glfw.lua"
    
    
    filter "system:windows"
        links { "GLFW", "GLAD", "GLM" }
        defines { "_WINDOWS" }
        include "libs/glfw.lua"
    
    
    filter { "system:macosx" }
        links {"glfw.3.3", "GLAD", "GLM"}
        defines { "_GLFW_COCOA" }
    
    filter {}

include "libs/glad.lua"
include "libs/glm.lua"
-- include "libs/imgui.lua"
