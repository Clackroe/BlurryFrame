#include <iostream>
#include <image/image_loader.h>
#include "image/image.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "graphics/window.hpp"
#include "graphics/shader.hpp"
#include <graphics/camera.hpp>


// settings
// const unsigned int SCR_WIDTH = 435;
// const unsigned int SCR_HEIGHT = 121;
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;


int main()
{
    // const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    Window glWindow = Window(SCR_WIDTH, SCR_HEIGHT, "BlurryFrame");

    Shader* shader = new Shader("assets/shaders/basic-vert.glsl", "assets/shaders/basic-frag.glsl");
    Camera* camera = new Camera(glm::vec3(0.0, 0.0, 20.0), ORTHO);

    //Texture stuff TODO: ABSTRACT HEAVILY
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    Image image = Image("test5.jpeg");
    // Image image2 = Image("test1.png");

    image.loadTexture(0);
    // image2.loadTexture(1);

    // render loop
    // -----------
    float angle = 0.0;
    while (!glfwWindowShouldClose(glWindow.window))

    {
        glWindow.frameStart();
        glWindow.Update();

        shader->use();
        shader->setInt("image", 0);

        // image scale based on screen size and image size
        float scale_factor = 1.0f;
        if ((image.w / image.h) < (SCR_WIDTH / SCR_HEIGHT)) { // image is wider than screen
            scale_factor = std::min(static_cast<float>(SCR_WIDTH) / image.w, static_cast<float>(SCR_HEIGHT) / image.h);
        } else { // image is taller than screen
            scale_factor = std::min(static_cast<float>(SCR_WIDTH) / image.w, static_cast<float>(SCR_HEIGHT) / image.h);
        }
        glm::mat4 trans = glm::scale(mat4(1.0f), vec3(scale_factor, scale_factor, 1.0));
        
        shader->setMat4("model", trans);
        shader->setMat4("proj", camera->getProjection());
        shader->setMat4("view", camera->getView());
        image.render();

        glWindow.frameEnd();
    }


    delete shader;
    return 0;
}

