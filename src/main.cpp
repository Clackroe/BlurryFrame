#include "graphics/renderer.hpp"
#include "graphics/shader.hpp"
#include "graphics/window.hpp"
#include "image/image.hpp"
#include <algorithm>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <filesystem>
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

    Renderer* rend = new Renderer(camera);

    // get all files in content folder
    std::string path = "content/";
    // get all files that are images
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        if (entry.path().extension() == ".mov" || entry.path().extension() == ".jpg" || entry.path().extension() == ".png" || entry.path().extension() == ".jpeg" || entry.path().extension() == ".JPG") {
            files.push_back(entry.path().string());
        }
    }

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(glWindow.window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    ImGui::StyleColorsDark();

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
    while (!glfwWindowShouldClose(glWindow.window)) {
        glWindow.frameStart();
        glWindow.Update();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Blur");
        ImGui::Text("Blurryyyy"); //****I DONT LIKE THIS IT UGLY!!!!!!*****
        ImGui::Text("FPS: %f", ImGui::GetIO().Framerate);
        ImGui::End();

        ImGui::Render();

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

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glWindow.frameEnd();

        if (counterToKeepTime == 2000) {
            counterToKeepTime = 0;
            renderImage(shuffledIndecies);
        }
        counterToKeepTime++;
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    // delete rend;
    // delete camera;
    return 0;
}

void renderImage(int shuffledIndecies[])
{
    currentImageIndex++;
    // if file is .mov
    if (files[shuffledIndecies[currentImageIndex]].find(".mov") != std::string::npos) {
        // do nothing
        std::cout << "MOVIE" << std::endl;
    } else {
        image = new Image(files[shuffledIndecies[currentImageIndex]].c_str());
        blurImage = new Image(files[shuffledIndecies[currentImageIndex]].c_str());
        image->loadTexture(0);
        blurImage->blur(20.0);
        blurImage->loadTexture(1);
    }
}
