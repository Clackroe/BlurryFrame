#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "graphics/shader.hpp"
#include "graphics/window.hpp"
#include "image/image.hpp"
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <filesystem>
#include <graphics/camera.hpp>
#include <image/image_loader.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>
// threading
#include <thread>
#include <chrono>
#include <mutex>

// Mutex to protect shared data
std::mutex mutex;
void renderImage(Window glWindow);
Image image = Image("content/test.jpeg");
Image blurImage = Image("content/test.jpeg");


    // std::cout << "Starting thread" << std::endl;
    // glfw: initialize and configure
Window glWindow = Window("blur");
Shader* shader = new Shader("assets/shaders/basic-vert.glsl", "assets/shaders/basic-frag.glsl");
Camera* camera = new Camera(glm::vec3(0.0, 0.0, 20.0), ORTHO);

int main()
{
    // Texture stuff TODO: ABSTRACT HEAVILY
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // // get all files in content folder
    // std::string path = "content/";
    // // array to hold path names
    // std::vector<std::string> files;
    // for (const auto& entry : std::filesystem::directory_iterator(path))
    //     files.push_back(entry.path().string());

    // ImGui::CreateContext();
    // ImGui_ImplGlfw_InitForOpenGL(glWindow.window, true);
    // ImGui_ImplOpenGL3_Init("#version 330");

    // ImGui::StyleColorsDark();

    // int i = 0;
    // Image image = Image(files[i].c_str());
    // Image blurImage = Image(files[i].c_str());
    // image.loadTexture(0);
    // blurImage.blur(15, 5.0);
    // blurImage.loadTexture(1);

    // std::cout << "Image width: " << image.w << " Image height: " << image.h << std::endl;
    // get all files in content folder
    // std::string path = "content/";
    // // array to hold path names
    // std::vector<std::string> files;
    // for (const auto& entry : std::filesystem::directory_iterator(path))
    //     files.push_back(entry.path().string());
    // Image image = Image(files[0].c_str());
    // Image blurImage = Image(files[0].c_str());

    // render loop
    // -----------
    // ImGui::CreateContext();
    // ImGui_ImplGlfw_InitForOpenGL(glWindow.window, true);
    // ImGui_ImplOpenGL3_Init("#version 330");
    // ImGui::StyleColorsDark();

    // start render thread
    std::thread t1(renderImage, std::ref(glWindow));
    sleep(10);
    float angle = 0.0;
    while (!glfwWindowShouldClose(glWindow.window))

    {
        glWindow.frameStart();
        glWindow.Update();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Blur");
        ImGui::Text("Blurryyyy");
        ImGui::Text("FPS: %f", ImGui::GetIO().Framerate);
        ImGui::End();

        ImGui::Render();

        // BlureImage
        float blur_scale = 1.0f;
        if (glWindow.getWidth() / blurImage.w < glWindow.getHeight() / blurImage.h) {
            blur_scale = (float)glWindow.getHeight() / (float)blurImage.h;
        } else {
            blur_scale = (float)glWindow.getWidth() / (float)blurImage.w;
        }
        glm::mat4 trans_blur = glm::scale(mat4(1.0f), vec3(blur_scale, blur_scale, 1.0));

        shader->use();
        shader->setInt("image", 1);

        shader->setMat4("model", trans_blur);
        shader->setMat4("proj", camera->getProjection());
        shader->setMat4("view", camera->getView());
        blurImage.render();

        // NormalImage
        shader->setInt("image", 0);
        float scale_factor = 1.0f;
        if ((image.w / image.h) < (glWindow.getHeight() / glWindow.getWidth())) { // image is wider than screen
            scale_factor = std::min(static_cast<float>(glWindow.getWidth()) / image.w, static_cast<float>(glWindow.getHeight()) / image.h);
        } else { // image is taller than screen
            scale_factor = std::min(static_cast<float>(glWindow.getWidth()) / image.w, static_cast<float>(glWindow.getHeight()) / image.h);
        }
        glm::mat4 trans = glm::scale(mat4(1.0f), vec3(scale_factor, scale_factor, 1.0));

        shader->setMat4("model", trans);
        shader->setMat4("proj", camera->getProjection());
        shader->setMat4("view", camera->getView());
        image.render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glWindow.frameEnd();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    t1.join();

    delete shader;
    return 0;
}

void renderImage(Window glWindow) {

    std::cout << "Rendering image" << std::endl;
    // Texture stuff TODO: ABSTRACT HEAVILY
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    
    std::cout << "test" << std::endl;
    // get all files in content folder
    std::string path = "content/";
    // array to hold path names
    std::vector<std::string> files;
    for (const auto& entry : std::filesystem::directory_iterator(path))
        files.push_back(entry.path().string());

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(glWindow.window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    ImGui::StyleColorsDark();

    int i = 0;
    Image image = Image(files[i].c_str());
    Image blurImage = Image(files[i].c_str());
    image.loadTexture(0);
    blurImage.blur(15, 5.0);
    blurImage.loadTexture(1);

    std::cout << "Image width: " << image.w << " Image height: " << image.h << std::endl;
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        mutex.lock();
        i++;
        if (i >= files.size()) {
            i = 0;
        }
        image = Image(files[i].c_str());
        std::cout << "Rendering image" << std::endl;
        blurImage = Image(files[i].c_str());
        image.loadTexture(0);
        blurImage.blur(15, 5.0);
        blurImage.loadTexture(1);
        mutex.unlock();
    }
}