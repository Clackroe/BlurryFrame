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
void operator delete[](void* memory, size_t size)
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

    std::string curr = curr_item.viewing ? "---" : curr_item.name;

    std::sprintf(buffer, "Image: %i %c %s ", index, loaded, curr.c_str());
    if (*output != nullptr) {
        free(output);
    }

    *output = strdup(buffer);

    return true;
};

void glCleanup(unsigned int* VAO, unsigned int* VBO, unsigned int* EBO)
{
    glDeleteVertexArrays(1, VAO);
    glDeleteBuffers(1, VBO);
    glDeleteBuffers(1, EBO);
}

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
        // Begin Debug
        ImGui::Begin("DEBUG");
        ImGui::Text("FPS: %f   CPU Time: %f", ImGui::GetIO().Framerate, cpuTime);
        ImGui::SeparatorText("Memory");
        // THIS ISN'T ACCURATE
        // ImGui::Text("Total Allocated Memory: %f MB", (float)totalAllocatedMemory / (1000000.0f));
        // ImGui::Text("Total Freed Memory: %f MB", (float)totalFreedMemory / (1000000.0f));
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

        // End Debug

        // Blur Image

        rend->setShader(basicShader);
        if (sMananger->imageToRender != nullptr) {
            float blur_scale = 1.0f;
            if (glWindow.getWidth() / sMananger->imageToRender->burImage.w < glWindow.getHeight() / sMananger->imageToRender->burImage.h) {
                blur_scale = (float)glWindow.getHeight() / sMananger->imageToRender->burImage.h;
            } else {
                blur_scale = (float)glWindow.getWidth() / sMananger->imageToRender->burImage.w;
            }
            //
            // NormalImage
            float scale_factor = 1.0f;
            if ((sMananger->imageToRender->image.w / sMananger->imageToRender->image.h) < (glWindow.getHeight() / glWindow.getWidth())) { // image is wider than screen
                scale_factor = std::min(static_cast<float>(glWindow.getWidth()) / sMananger->imageToRender->image.w, static_cast<float>(glWindow.getHeight()) / sMananger->imageToRender->image.h);
            } else { // image is taller than screen
                scale_factor = std::min(static_cast<float>(glWindow.getWidth()) / sMananger->imageToRender->image.w, static_cast<float>(glWindow.getHeight()) / sMananger->imageToRender->image.h);
            }
            if (sMananger->imageToRender->image.pixels != nullptr) {
                sMananger->imageToRender->image.loadTexture(0);
            }
            // std::cout << scale_factor << std::endl;
            if (sMananger->imageToRender->burImage.pixels != nullptr) {
                sMananger->imageToRender->burImage.loadTexture(1);
            }
            sMananger->imageToRender->burImage.transform.scale = { blur_scale, blur_scale, 1.0f };
            sMananger->imageToRender->image.transform.scale = { scale_factor, scale_factor, 1.0f };
            rend->renderImage(sMananger->imageToRender->burImage);
            rend->renderImage(sMananger->imageToRender->image);
        }

        // for (GlCleanupItem item : sMananger->glCleanupBuffer) {
        //     glCleanup(item.VAO, item.VBO, item.EBO);
        // }

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
