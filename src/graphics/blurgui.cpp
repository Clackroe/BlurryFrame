#include <graphics/blurgui.hpp>

BlurGui::BlurGui(Window* win)
{
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(win->window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    ImGui::StyleColorsDark();
};

BlurGui::~BlurGui()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void BlurGui::frameStart()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void BlurGui::frameEnd()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
