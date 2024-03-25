#include "graphics/renderer.hpp"
#include "graphics/shader.hpp"
#include "graphics/window.hpp"
#include "image/image.hpp"
#include <algorithm>
#include <filesystem>
#include <graphics/blurgui.hpp>
#include <graphics/camera.hpp>
#include <unistd.h>
#include <vector>

void renderImage(int shuffledIndecies[]);
Image* image;
Image* blurImage;
int currentImageIndex;
std::vector<std::string> files;

int main()
{
    // Window creation
    Window glWindow = Window("blur");
    Shader* basicShader = new Shader("assets/shaders/basic-vert.glsl", "assets/shaders/basic-frag.glsl");
    Camera* camera = new Camera(glm::vec3(0.0, 0.0, 20.0), ORTHO);

    BlurGui* gui = new BlurGui(&glWindow);
    Renderer* rend = new Renderer(camera);

    // get all files in content folder
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

    renderImage(shuffledIndecies);

    int counterToKeepTime = 0;
    int angle;
    while (!glfwWindowShouldClose(glWindow.window)) {
        glWindow.frameStart();
        glWindow.Update();
        gui->frameStart();

        ImGui::Begin("Blur");
        ImGui::Text("FPS: %f", ImGui::GetIO().Framerate);
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
        rend->renderImage(*blurImage);
        rend->renderImage(*image);

        gui->frameEnd();
        glWindow.frameEnd();

        if (counterToKeepTime == 2000) {
            counterToKeepTime = 0;
            renderImage(shuffledIndecies);
        }
        counterToKeepTime++;
        angle++;
    }

    delete rend;
    delete camera;
    delete basicShader;
    delete gui;
    return 0;
}

void renderImage(int shuffledIndecies[])
{
    currentImageIndex++;
    image = new Image(files[shuffledIndecies[currentImageIndex]].c_str());
    blurImage = new Image(files[shuffledIndecies[currentImageIndex]].c_str());
    image->loadTexture(0);
    blurImage->blur(20.0);
    blurImage->loadTexture(1);
}
