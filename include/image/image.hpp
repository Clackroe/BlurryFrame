#ifndef IMAGE_H
#define IMAGE_H
#include <image/image_loader.h>
#include <glad/glad.h>
#include <glm/glm.hpp>

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

private:
    const char* path;
    void generateVertex();
    unsigned int VBO, VAO, EBO;




};



#endif

