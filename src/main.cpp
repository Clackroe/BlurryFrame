#include <iostream>
#include <image/image_loader.h>
#include "glm/ext/matrix_transform.hpp"
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
    Window glWindow = Window(SCR_WIDTH, SCR_HEIGHT, "BlurryFrame");

    Shader* shader = new Shader("assets/shaders/basic-vert.glsl", "assets/shaders/basic-frag.glsl");
    Camera* camera = new Camera(glm::vec3(0.0, 0.0, 3.0), ORTHO);

    float vertexPos[] = {
         1.0f,  1.0f, 0.0f,  // top right
         1.0f, -1.0f, 0.0f,  // bottom right
        -1.0f, -1.0f, 0.0f,  // bottom left
        -1.0f,  1.0f, 0.0f   // top left 
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };

    float textureCoords[] = {
        1.0f, 1.0f, // top right
        1.0f, 0.0f,// lower right
        0.0f, 0.f, // lower left
        0.0f, 1.0f // top left
    };



    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);
    
    struct Vertex {
        //aPos in shader
        float x;
        float y;
        float z;
        // aTexPos in shader
        float u;
        float v;
    };

    Vertex vertices[4];
    
    //Fill the vertex array with the appropriate data. Will Be abstracted. 
    for (int i = 0; i < 4; ++i) {
        int texIndex = i * 2;
        int posIndex = i * 3;
        
        vertices[i].x = vertexPos[posIndex];
        vertices[i].y = vertexPos[posIndex + 1];
        vertices[i].z = vertexPos[posIndex + 2];
        
        vertices[i].u = textureCoords[texIndex];
        vertices[i].v = textureCoords[texIndex + 1];
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,  sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 


    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0); 


    //Texture stuff TODO: ABSTRACT HEAVILY
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //Load the Texture
    const char* path = "test.png";
    int w, h, chan;
    unsigned char* imageData = Loader::loadImage(path, &w, &h, &chan); 
    //This currently provides no advantage to using STB directly. 
    //Abstract to return an Image struct? TBD


    unsigned int texture;
    glGenTextures(1, &texture);
    
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
    glGenerateMipmap(GL_TEXTURE_2D);
    //TODO: Abstract most of this texture boilerplate.
    
    Loader::freePixels(imageData);

    glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
    glBindTexture(GL_TEXTURE_2D, texture);
    //
    // render loop
    // -----------
    while (!glfwWindowShouldClose(glWindow.window))

    {
        glWindow.frameStart();
        glWindow.Update();

        glBindTexture(GL_TEXTURE_2D, texture);

        shader->use();
        shader->setInt("image", 0);
        glm::mat4 trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        // trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
        // trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));        
        // glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        shader->setMat4("model", trans);
        shader->setMat4("proj", camera->getProjection());
        // shader->setMat4("view", glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, -3.0)));
        shader->setMat4("view", camera->getView());

        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glWindow.frameEnd();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    delete shader;
    return 0;
}

