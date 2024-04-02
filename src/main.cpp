#include "graphics/renderer.hpp"
#include "graphics/shader.hpp"
#include "graphics/window.hpp"
#include "image/image.hpp"
#include "imgui.h"
#include <cstdio>
#include <graphics/blurgui.hpp>
#include <graphics/camera.hpp>
#include <management/slide_manager.hpp>
#include <string>
#include <unistd.h>
//
size_t totalAllocatedMemory = 0;
size_t totalFreedMemory = 0;
float cpuTime = 0;

void* operator new(size_t size)
{
    totalAllocatedMemory += size;
    return malloc(size);
}

void operator delete(void* memory, size_t size)
{
    totalFreedMemory += size;
    free(memory);
}

bool loadQueueGetter(void* data, int index, const char** output)
{
    QueueItem* items = (QueueItem*)data;
    QueueItem& curr_item = items[index];

    char buffer[50];

    char loaded = curr_item.shouldLoad ? 'Y' : 'N';

    std::string curr = curr_item.viewing ? "---" : " ";

    std::sprintf(buffer, "Image: %i Seen: %c %s ", index, loaded, curr.c_str());
    if (*output != nullptr) {
        free(output);
    }

    *output = strdup(buffer);

    return true;
};

int main()
{
    Window glWindow = Window("blur");
    Shader* basicShader = new Shader("assets/shaders/basic-vert.glsl", "assets/shaders/basic-frag.glsl");
    Camera* camera = new Camera(glm::vec3(0.0, 0.0, 20.0), ORTHO);

    BlurGui* gui = new BlurGui(&glWindow);
    Renderer* rend = new Renderer(camera);

    SlideManager* sMananger = new SlideManager(10, 3);

    sMananger->run();

    static int currentQueueItem;
    float t = 0;
    while (!glfwWindowShouldClose(glWindow.window)) {
        glWindow.frameStart();
        glWindow.Update();
        gui->frameStart();
        t = glfwGetTime();

        ImGui::Begin("DEBUG");
        ImGui::Text("FPS: %f   CPU Time: %f", ImGui::GetIO().Framerate, cpuTime);
        ImGui::SeparatorText("Memory");
        ImGui::Text("Total Allocated Memory: %f MB", (float)totalAllocatedMemory / (1000000.0f));
        ImGui::Text("Total Freed Memory: %f MB", (float)totalFreedMemory / (1000000.0f));
        ImGui::SeparatorText("SlideManager Debug");
        ImGui::Text("Timer Length %i", sMananger->timerLength);
        ImGui::Text("Elapsed: %f", sMananger->timer.getElapsedTime());
        ImGui::Text("Timer Count: %d", sMananger->timeCount);
        ImGui::NewLine();
        ImGui::Text("BuffSize: %i: ", sMananger->buffSize);
        ImGui::Text("Current Image: %i: ", sMananger->currentImageIndex);
        // ImGui::Button("Prev", { 100, 50 });
        if (ImGui::Button("Prev", { 50, 25 })) {
            sMananger->prev();
        }
        if (ImGui::Button("Next", { 50, 25 })) {
            sMananger->next();
        }

        ImGui::Text("Load Queue");
        ImGui::ListBox(
            "##Load Queue",
            &currentQueueItem,
            loadQueueGetter,
            sMananger->loadQueue,
            sMananger->buffSize);
        ImGui::End();

        // Blur Image
        // float blur_scale = 1.0f;
        // if (glWindow.getWidth() / blurImage->w < glWindow.getHeight() / blurImage->h) {
        //     blur_scale = (float)glWindow.getHeight() / blurImage->h;
        // } else {
        //     blur_scale = (float)glWindow.getWidth() / blurImage->w;
        // }
        // // NormalImage
        // float scale_factor = 1.0f;
        // if ((image->w / image->h) < (glWindow.getHeight() / glWindow.getWidth())) { // image is wider than screen
        //     scale_factor = std::min(static_cast<float>(glWindow.getWidth()) / image->w, static_cast<float>(glWindow.getHeight()) / image->h);
        // } else { // image is taller than screen
        //     scale_factor = std::min(static_cast<float>(glWindow.getWidth()) / image->w, static_cast<float>(glWindow.getHeight()) / image->h);
        // }

        rend->setShader(basicShader);
        if (sMananger->imageToRender != nullptr) {
            if (sMananger->imageToRender->pixels != nullptr) {
                sMananger->imageToRender->loadTexture(0);
            }
            rend->renderImage(*sMananger->imageToRender);
        }

        cpuTime = glfwGetTime() - t;
        gui->frameEnd();
        glWindow.frameEnd();
    }

    delete rend;
    delete camera;
    delete basicShader;
    delete gui;
    sMananger->close();
    delete sMananger;

    std::cout << "Final Mem Usage: " << totalAllocatedMemory - totalFreedMemory << std::endl;
    return 0;
}
