#ifndef IMAGE_H
#define IMAGE_H
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <image/image_loader.h>

struct Vertex {
    glm::vec3 pos;
    glm::vec2 texUV;
};

class Image {

public:
    Image(const char* path);
    ~Image();
    int w, h, chan;
    unsigned char* pixels;
    unsigned int texID;
    int textureSlot;

    void loadTexture(int textureSlot);

    void frameStart();
    void render();
    void frameEnd();

    void blur(int rad, float sigma);

    unsigned int VBO, VAO, EBO;

    glm::vec3 position, rotation, scale;

    glm::mat4 getRotationMat() { return rotationMatrix; };
    glm::mat4 getScaleMat() { return scaleMatrix; };
    glm::mat4 getPositionMat() { return positionMatrix; };

private:
    const char* path;
    void generateVertex();
    glm::mat4 rotationMatrix, scaleMatrix, positionMatrix;
};

#endif
