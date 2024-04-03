#include "glm/ext/scalar_constants.hpp"
#include "graphics/window.hpp"
#include "image/image_utils.hpp"
#include <image/image.hpp>
#include <iostream>
#include <iterator>
#include <ostream>

Image::Image()
{
    path = "--";
    VBO = 0;
    VAO = 0;
    EBO = 0;

    transform.position = glm::vec3(0.0f, 0.0f, 0.0f);
    transform.scale = glm::vec3(1.0f, 1.0f, 1.0f);
    transform.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    pixels = nullptr;
}

Image::Image(std::string path)
{
    Image::path = path;
    pixels = Loader::loadImage(path.data(), &w, &h, &chan);
    transform.position = glm::vec3(0.0f, 0.0f, 0.0f);
    transform.scale = glm::vec3(1.0f, 1.0f, 1.0f);
    transform.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
}
Image::~Image()
{
    if (pixels != nullptr) {
        Loader::freePixels(pixels);
        pixels = nullptr;
    }
    if (VAO > 0)
        glDeleteVertexArrays(1, &VAO);
    if (VBO > 0)
        glDeleteBuffers(1, &VBO);
    if (EBO > 0)
        glDeleteBuffers(1, &EBO);
}

void Image::reconstruct(char* path)
{
    Image::path = path;
    pixels = Loader::loadImage(path, &w, &h, &chan);
    transform.position = glm::vec3(0.0f, 0.0f, 0.0f);
    transform.scale = glm::vec3(1.0f, 1.0f, 1.0f);
    transform.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
}

void Image::deconstruct()
{
    if (pixels != nullptr) {
        Loader::freePixels(pixels);
        pixels = nullptr;
    }
    path = "---";

    transform.position = glm::vec3(0.0f, 0.0f, 0.0f);
    transform.scale = glm::vec3(1.0f, 1.0f, 1.0f);
    transform.rotation = glm::vec3(0.0f, 0.0f, 0.0f);

    model = glm::mat4(1.0);
    prevTransform = Transform();

    w = 0;
    h = 0;
    chan = 0;
    texID = -1;
    textureSlot = -1;
    imageLoaded = false;
}

void Image::blur(float sigma)
{
    int nW = 720;
    int nH = 1280;
    unsigned char* outputPixels = new unsigned char[nW * nH * 3];
    Blur::downsample_image(pixels, outputPixels, w, h, nW, nH);
    w = nW;
    h = nH;

    int rad = std::ceil(2 * sigma);
    pixels = new unsigned char[(w - rad * 2) * (h - rad * 2) * 3];
    Blur::applyGaussianFilter(outputPixels, pixels, w, h, chan, rad, sigma);
    w -= (rad * 2);
    h -= (rad * 2);
    Loader::freePixels(outputPixels);
}

void Image::loadTexture(int textureSlot)
{
    if (imageLoaded) {
        return;
    }
    Image::textureSlot = textureSlot;

    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    glGenerateMipmap(GL_TEXTURE_2D);

    glActiveTexture(GL_TEXTURE0 + textureSlot);
    glBindTexture(GL_TEXTURE_2D, texID);

    imageLoaded = true;

    loadToGPU();
}

void Image::generateVertex()
{
    float halfWidth = w / 2.0f;
    float halfHeight = h / 2.0f;

    float vertexPos[] = {
        halfWidth, halfHeight, 0.0f, // top right
        halfWidth, -halfHeight, 0.0f, // bottom right
        -halfWidth, -halfHeight, 0.0f, // bottom left
        -halfWidth, halfHeight, 0.0f // top left
    };

    // Normalize vertex positions to be between 0 and 1
    for (int i = 0; i < 4; ++i) {
        vertexPos[i * 3] /= Window::getWidth();
        vertexPos[i * 3 + 1] /= Window::getHeight();
    }

    for (int i = 0; i < 4; ++i) {
        int texIndex = i * 2;
        int posIndex = i * 3;

        vertices[i].pos.x = vertexPos[posIndex];
        vertices[i].pos.y = vertexPos[posIndex + 1];
        vertices[i].pos.z = vertexPos[posIndex + 2];

        vertices[i].texUV.x = textureCoords[texIndex];
        vertices[i].texUV.y = textureCoords[texIndex + 1];
    }
}

void Image::loadToGPU()
{
    if (VAO <= 0)
        glDeleteVertexArrays(1, &VAO);
    if (VBO <= 0)
        glDeleteBuffers(1, &VBO);
    if (EBO <= 0)
        glDeleteBuffers(1, &EBO);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // Unbind VAO
    glBindVertexArray(0);
}
