#ifndef IMAGE_H
#define IMAGE_H
#include "glm/ext/matrix_transform.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <image/image_loader.h>
#include <string>

struct Vertex {
    glm::vec3 pos;
    glm::vec2 texUV;
};

struct Transform {
    glm::vec3 position, rotation, scale;
};

class Image {

public:
    Image();
    Image(std::string path);
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

    void deconstruct();
    void reconstruct(char* path);

    void generateVertex();
    void loadToGPU();

    void blur(float sigma);

    unsigned int VBO, VAO, EBO;

    glm::mat4 getModelMatrix()
    {
        if (modelChanged()) {

            glm::mat4 mModel = glm::mat4(1.0f);
            mModel = glm::rotate(mModel, glm::radians(transform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
            mModel = glm::rotate(mModel, glm::radians(transform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
            mModel = glm::rotate(mModel, glm::radians(transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

            mModel = glm::scale(mModel, transform.scale);

            mModel = glm::translate(mModel, transform.position);

            model = mModel;
            prevTransform = transform;
            return mModel;
        } else {
            return model;
        }
    };

private:
    std::string path;
    glm::mat4 model;
    Transform prevTransform;

    bool modelChanged()
    {
        return (prevTransform.position != transform.position || prevTransform.scale != transform.scale || prevTransform.rotation != transform.rotation);
    };

    Vertex vertices[4];
    unsigned int indices[6] = {
        0, 1, 3, // first Triangle
        1, 2, 3 // second Triangle
    };
    float textureCoords[8] = {
        1.0f, 1.0f, // top right
        1.0f, 0.0f, // lower right
        0.0f, 0.0f, // lower left
        0.0f, 1.0f // top left
    };
};

#endif
