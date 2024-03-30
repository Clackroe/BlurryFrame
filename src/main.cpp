#include "graphics/renderer.hpp"
#include "graphics/shader.hpp"
#include "graphics/window.hpp"
#include "image/image.hpp"
#include "imgui.h"
#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <graphics/blurgui.hpp>
#include <graphics/camera.hpp>
#include <management/slide_manager.hpp>
#include <string>
#include <unistd.h>
#include <vector>
//
size_t totalAllocatedMemory = 0;
size_t totalFreedMemory = 0;
float cpuTime = 0;

// void* operator new(size_t size)
// {
//     totalAllocatedMemory += size;
//     return malloc(size);
// }
//
// void operator delete(void* memory, size_t size)
// {
//     totalFreedMemory += size;
//     free(memory);
// }
//
void renderImage();
Image* image;
Image* blurImage;
int currentImageIndex;
std::vector<std::string> files;

bool loadQueueGetter(void* data, int index, const char** output)
{
    QueueItem* items = (QueueItem*)data;
    QueueItem& curr_item = items[index];

    char buffer[50];

    char loaded = curr_item.loaded ? 'L' : 'U';
    std::string curr = curr_item.viewing ? "---" : " ";

    std::sprintf(buffer, "Image: %i Seen: %d %c %s ", index, curr_item.seen, loaded, curr.c_str());

    *output = strdup(buffer);

    return true;
};

int main()
{
    // Window creation
    Window glWindow = Window("blur");
    Shader* basicShader = new Shader("assets/shaders/basic-vert.glsl", "assets/shaders/basic-frag.glsl");
    Camera* camera = new Camera(glm::vec3(0.0, 0.0, 20.0), ORTHO);

    BlurGui* gui = new BlurGui(&glWindow);
    Renderer* rend = new Renderer(camera);

    SlideManager* sMananger = new SlideManager(10, 5);

    // get all files in content folder
    //
    std::string path = "content/";
    // get all files that are images
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        if (entry.path().extension() == ".jpg" || entry.path().extension() == ".png" || entry.path().extension() == ".jpeg" || entry.path().extension() == ".JPG") {
            files.push_back(entry.path().string());
        }
    }

    // render first image
    currentImageIndex = 0;
    int sizeOfFiles = files.size();
    int shuffledIndecies[sizeOfFiles];
    for (int i = 0; i < sizeOfFiles; i++) {
        shuffledIndecies[i] = i;
    }
    // shuffle the indecies
    for (int i = 0; i < sizeOfFiles; i++) {
        int randomIndex = rand() % sizeOfFiles;
        int temp = shuffledIndecies[i];
        shuffledIndecies[i] = shuffledIndecies[randomIndex];
        shuffledIndecies[randomIndex] = temp;
    }

    renderImage();
    ImGuiIO& io = ImGui::GetIO();
    sMananger->run();
    static int currentQueueItem;
    int counterToKeepTime = 0;
    int angle;
    float t = 0;
    while (!glfwWindowShouldClose(glWindow.window)) {
        glWindow.frameStart();
        glWindow.Update();
        gui->frameStart();
        t = glfwGetTime();

        ImGui::Begin("DEBUG");
        ImGui::Text("FPS: %f   CPU Time: %f", ImGui::GetIO().Framerate, cpuTime);
        ImGui::SeparatorText("SlideManager Debug");
        ImGui::Text("Timer Length %i", sMananger->timerLength);
        ImGui::Text("Elapsed: %f", sMananger->timer.getElapsedTime());
        ImGui::Text("Timer Count: %d", sMananger->timeCount);
        ImGui::NewLine();
        ImGui::Text("BuffSize: %i: ", sMananger->buffSize);
        ImGui::Text("Current Image: %i: ", sMananger->currentImageIndex);
        ImGui::Text("Load Queue");
        ImGui::ListBox(
            "##Load Queue",
            &currentQueueItem,
            loadQueueGetter,
            sMananger->loadQueue,
            sMananger->buffSize);

        // ImGui::SeparatorText("Memory");
        // ImGui::Text("Total Allocated Memory: %f MB", (float)totalAllocatedMemory / (1000000.0f));
        // ImGui::Text("Total Freed Memory: %f MB", (float)totalFreedMemory / (1000000.0f));
        ImGui::End();

        // BlureImage
        float blur_scale = 1.0f;
        if (glWindow.getWidth() / blurImage->w < glWindow.getHeight() / blurImage->h) {
            blur_scale = (float)glWindow.getHeight() / blurImage->h;
        } else {
            blur_scale = (float)glWindow.getWidth() / blurImage->w;
        }
        blurImage->transform.scale = glm::vec3(blur_scale, blur_scale, 1.0f);

        // NormalImage
        float scale_factor = 1.0f;
        if ((image->w / image->h) < (glWindow.getHeight() / glWindow.getWidth())) { // image is wider than screen
            scale_factor = std::min(static_cast<float>(glWindow.getWidth()) / image->w, static_cast<float>(glWindow.getHeight()) / image->h);
        } else { // image is taller than screen
            scale_factor = std::min(static_cast<float>(glWindow.getWidth()) / image->w, static_cast<float>(glWindow.getHeight()) / image->h);
        }

        image->transform.scale = glm::vec3(scale_factor, scale_factor, 1.0f);

        rend->setShader(basicShader);

        if (sMananger->imageToRender != nullptr) {
            if (sMananger->imageToRender->pixels != nullptr) {
                sMananger->imageToRender->loadTexture(0);
            }
            rend->renderImage(*sMananger->imageToRender);
        }
        // rend->renderImage(*blurImage);
        // rend->renderImage(*image);

        cpuTime = glfwGetTime() - t;
        gui->frameEnd();
        glWindow.frameEnd();

        if (counterToKeepTime == 2000) {
            counterToKeepTime = 0;
            renderImage();
        }
        // counterToKeepTime++;
        angle++;
    }

    delete image;
    delete rend;
    delete camera;
    delete basicShader;
    delete gui;
    delete sMananger;

    std::cout << "Final Mem Usage: " << totalAllocatedMemory - totalFreedMemory << std::endl;
    return 0;
}

void renderImage()
{
    // printf("Image: %s ", files[shuffledIndecies[currentImageIndex]].data());
    // currentImageIndex++;
    image = new Image(files[0]);
    blurImage = new Image(files[0]);
    // image->loadTexture(0);
    // blurImage->blur(20.0);
    // blurImage->loadTexture(1);
}
