require "export-compile-commands"
workspace "Blurry"
    configurations { "Debug", "Release" }
    location "build"
    language "C++"
    architecture "x86_64"
    targetdir "bin/%{cfg.buildcfg}"
    objdir "obj/%{cfg.buildcfg}"


    includedirs {
        "include",
        "vendor",
        "vendor/SFML/include"
    }

    files {
        "src/**.h",
        "src/**.cpp"
    }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"

project "Blurry"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    staticruntime "On"
    toolset "clang"

    libdirs { "vendor/SFML/build/lib" }

    links {
      "sfml-graphics",  -- Link against SFML graphics library
      "sfml-window",    -- Link against SFML window library
      "sfml-system"     -- Link against SFML system library
   }

    files { "src/**.h", "src/**.cpp" }

    filter "system:macosx"
        systemversion "14.3.1"  -- Specify macOS 14 (Sanoma 14.3.1)
