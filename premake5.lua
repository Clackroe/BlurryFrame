-- require "export-compile-commands"
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
    
    files { "src/*.cpp" }

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
