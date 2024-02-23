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

    includedirs {"include/", "libs/glad/include/", "libs/glfw/include/", "libs/glm/"}
    
    files {
        "src/**.cpp",
        "src/image/**.cpp",
        "src/graphics/**.cpp",
        "include/**.hpp",
        "include/**.h"
    }

    filter "system:linux"
        links { "GLFW", "GLAD", "GLM", "dl", "pthread" }
        defines { "_X11" }
        include "libs/glfw.lua"
        include "libs/glad.lua"
        include "libs/glm.lua"
    
    filter "system:windows"
        links { "GLFW", "GLAD", "GLM" }
        defines { "_WINDOWS" }
        include "libs/glfw.lua"
    
    filter "system:macosx"
        links {"libs/glfw.3.3", "GLAD", "GLM"}
        defines { "_GLFW_COCOA" }
    
    filter {}

include "libs/glad.lua"
include "libs/glm.lua"
-- include "libs/imgui.lua"

newaction {
    trigger = "deep-clean",
    description = "Remove all binaries and intermediate binaries, and vs files.",
    execute = function()
        print("Removing binaries")
        os.rmdir("./bin")
        print("Removing intermediate binaries")
        os.rmdir("./obj")
        print("Removing project files")
        os.rmdir("./.vs")
        os.remove("**.sln")
        os.remove("**.vcxproj")
        os.remove("**.vcxproj.filters")
        os.remove("**.vcxproj.user")
        os.remove("./Makefile")
        os.remove("./*.make")
        os.remove("libs/*.make")
        print("Done")
    end
}
newaction {
    trigger = "clean",
    description = "Remove all binaries and intermediate binaries, and vs files.",
    execute = function()
        print("Removing binaries")
        os.rmdir("./bin")
        print("Removing intermediate binaries")
        os.rmdir("./obj")
        print("Removing project files")
        os.rmdir("./.vs")
        os.remove("**.sln")
        os.remove("**.vcxproj")
        os.remove("**.vcxproj.filters")
        os.remove("**.vcxproj.user")
        print("Done")
    end
}

newaction {
    trigger = "run",
    description = "Run in Debug Mode",
    execute = function ()
        print("Running DEBUG...")
        os.execute("./bin/Debug/Blurry")
    end
}

newaction {
    trigger = "build",
    description = "Build in debug mode",
    execute = function ()
        print("Building in DEBUG")
        os.execute("make")
    end
}
