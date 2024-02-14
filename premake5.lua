
workspace "Blurry"
    configurations { "Debug", "Release" }
    location "build"
    language "C++"
    architecture "x86_64"
    targetdir "bin/%{cfg.buildcfg}"
    objdir "obj/%{cfg.buildcfg}"

    includedirs {
        "include",
        "vendor/include"
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
    systemversion "latest"

    files { "src/**.h", "src/**.cpp" }
