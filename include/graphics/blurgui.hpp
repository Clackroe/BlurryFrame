
#ifndef IMGUI_HPP
#define IMGUI_HPP

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <graphics/window.hpp>

class BlurGui {
public:
    BlurGui(Window* win);
    ~BlurGui();

    void frameStart();
    void frameEnd();
};

#endif
