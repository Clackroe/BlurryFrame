#ifndef IMAGE_H
#define IMAGE_H
#include "glm/ext/matrix_transform.hpp"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <image/image_loader.h>

struct Vertex {
    glm::vec3 pos;
    glm::vec2 texUV;
};

struct Transform {
    glm::vec3 position, rotation, scale;
};

class Image {

public:
    Image(const char* path);
    ~Image();
    int w, h, chan;
    unsigned char* pixels;
    unsigned int texID;
    int textureSlot;

    Transform transform;

    void loadTexture(int textureSlot);

    void frameStart();
    void render();
    void frameEnd();

    void blur(float sigma);

    unsigned int VBO, VAO, EBO;

    void position(glm::vec3 pos)
    {
        transform.position = pos;
        modelChanged = true;
    };
    void rotation(glm::vec3 rot)
    {
        transform.rotation = rot;
        modelChanged = true;
    };
    void scale(glm::vec3 sc)
    {
        transform.scale = sc;
        modelChanged = true;
    };

    glm::mat4 getModelMatrix()
    {
        if (modelChanged) {

            glm::mat4 mModel = glm::mat4(1.0f);
            mModel = glm::rotate(mModel, glm::radians(transform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
            mModel = glm::rotate(mModel, glm::radians(transform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
            mModel = glm::rotate(mModel, glm::radians(transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

            mModel = glm::scale(mModel, transform.scale);

            mModel = glm::translate(mModel, transform.position);

            model = mModel;
            modelChanged = false;
            return mModel;
        } else {
            return model;
        }
    };

private:
    const char* path;
    void generateVertex();
    glm::mat4 model;
    bool modelChanged = true;
};

#endif
