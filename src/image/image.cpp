#include "glm/ext/scalar_constants.hpp"
#include "graphics/window.hpp"
#include "image/image_loader.h"
#include <image/image.hpp>
#include <iostream>
#include <iterator>
#include <ostream>
#include <vector>

Image::Image()
{
    // printf("Default Constructor\n");
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
    // printf("Constructor\n");
    Image::path = path;
    pixels = Loader::loadImage(path.data(), &w, &h, &chan);
    transform.position = glm::vec3(0.0f, 0.0f, 0.0f);
    transform.scale = glm::vec3(1.0f, 1.0f, 1.0f);
    transform.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
}
Image::~Image()
{
    // printf("destroying\n");
    if (pixels != nullptr) {
        Loader::freePixels(pixels);
        pixels = nullptr;
    }
    if (VAO != 0)
        glDeleteVertexArrays(1, &VAO);
    if (VBO != 0)
        glDeleteBuffers(1, &VBO);
    if (EBO != 0)
        glDeleteBuffers(1, &EBO);

    // printf("destroyed\n");
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

    // std::cout << "Destruct" << std::endl;
    path = "---";

    // glDeleteVertexArrays(1, &VAO);
    // glDeleteBuffers(1, &VBO);
    // glDeleteBuffers(1, &EBO);

    transform.position = glm::vec3(0.0f, 0.0f, 0.0f);
    transform.scale = glm::vec3(1.0f, 1.0f, 1.0f);
    transform.rotation = glm::vec3(0.0f, 0.0f, 0.0f);

    model = glm::mat4(1.0);

    if (pixels != nullptr) {
        Loader::freePixels(pixels);
        std::cout << "After Free Pixels" << std::endl;
    }

    w = 0;
    h = 0;
    chan = 0;
    texID = -1;
    textureSlot = -1;
}

// 1D Gaussian to take advantage of seperable filters.
static std::vector<float> genGaussianKernal1D(int radius, float sigma)
{
    std::vector<float> kernel;
    float sum = 0.0f;
    for (int i = -radius; i <= radius; ++i) {
        float x = i;
        float weight = exp(-x * x / (2 * sigma * sigma)) / (sqrt(2.0f * glm::pi<float>()) * sigma);
        kernel.push_back(weight);
        sum += weight;
    }
    for (float& val : kernel) {
        val /= sum;
    }
    return kernel;
}

static unsigned char* downsample_image(unsigned char* image, int width, int height, int newWidth, int newHeight)
{
    int new_width = newWidth;
    int new_height = newHeight;

    unsigned char* new_image = new unsigned char[new_width * new_height * 3];

    for (int y = 0; y < new_height; y++) {
        for (int x = 0; x < new_width; x++) {
            int original_x = (x * width) / new_width;
            int original_y = (y * height) / new_height;

            new_image[(y * new_width + x) * 3] = image[(original_y * width + original_x) * 3];
            new_image[(y * new_width + x) * 3 + 1] = image[(original_y * width + original_x) * 3 + 1];
            new_image[(y * new_width + x) * 3 + 2] = image[(original_y * width + original_x) * 3 + 2];
        }
    }

    return new_image;
}

static void applyGaussianFilter(const unsigned char* inputPixels, unsigned char* outputPixels,
    int width, int height, int channels, int radius, float sigma)
{
    // Utilizing seperable filters for optimization.
    // Without seperability O(N*M*K^2)
    // With seperability    O(2N*M*K) = O(N*M*K) Faster by a bit.
    std::vector<float> kernel = genGaussianKernal1D(radius, sigma);

    // TODO: Consider creating a "step" value to step accross a certain amount
    // of pixels to essentially downsample before convolving.

    // Apply in the horizontal axis
    for (int y = radius; y < height - radius; y += 1) {
        for (int x = radius; x < width - radius; x += 1) {
            int pixelIndex = (y * width * 3) + x * 3;
            float sumR = 0.0f;
            float sumG = 0.0f;
            float sumB = 0.0f;

            float sumWeights = 0.0f;

            int kIndex = 0;
            for (int kx = -radius; kx <= radius; kx++) {
                int kPIndex = ((y)*width * channels) + (x + kx) * channels;
                sumR += inputPixels[kPIndex + 0] * kernel[kIndex];
                sumG += inputPixels[kPIndex + 1] * kernel[kIndex];
                sumB += inputPixels[kPIndex + 2] * kernel[kIndex];
                sumWeights += kernel[kIndex];
                kIndex++;
            }

            pixelIndex = ((y - radius) * (width - radius * 2) * 3) + (x - radius) * 3;
            outputPixels[pixelIndex + 0] = glm::clamp((sumR / sumWeights), 0.0f, 255.0f);
            outputPixels[pixelIndex + 1] = glm::clamp((sumG / sumWeights), 0.0f, 255.0f);
            outputPixels[pixelIndex + 2] = glm::clamp((sumB / sumWeights), 0.0f, 255.0f);
        }
    }

    // Apply kernel in the vertical.
    for (int y = radius; y < height - radius; y += 1) {
        for (int x = radius; x < width - radius; x += 1) {
            int pixelIndex = (y * width * 3) + x * 3;
            float sumR = 0.0f;
            float sumG = 0.0f;
            float sumB = 0.0f;

            float sumWeights = 0.0f;

            int kIndex = 0;
            for (int ky = -radius; ky <= radius; ky++) {
                int kPIndex = ((y + ky - radius) * (width - radius * 2) * channels) + (x - radius) * channels;
                sumR += outputPixels[kPIndex + 0] * kernel[kIndex];
                sumG += outputPixels[kPIndex + 1] * kernel[kIndex];
                sumB += outputPixels[kPIndex + 2] * kernel[kIndex];
                sumWeights += kernel[kIndex];
                kIndex++;
            }

            pixelIndex = ((y - radius) * (width - radius * 2) * 3) + (x - radius) * 3;
            outputPixels[pixelIndex + 0] = glm::clamp((sumR / sumWeights), 0.0f, 255.0f);
            outputPixels[pixelIndex + 1] = glm::clamp((sumG / sumWeights), 0.0f, 255.0f);
            outputPixels[pixelIndex + 2] = glm::clamp((sumB / sumWeights), 0.0f, 255.0f);
        }
    }
}

void Image::blur(float sigma)
{
    int nW = 720;
    int nH = 1280;
    // Higher Quality, Less Performance
    // int nW = 1080;
    // int nH = 1920;
    unsigned char* outputPixels = downsample_image(pixels, w, h, nW, nH);
    w = nW;
    h = nH;

    int rad = std::ceil(2 * sigma);
    unsigned char* nPix = new unsigned char[(w - rad * 2) * (h - rad * 2) * 3];
    applyGaussianFilter(outputPixels, nPix, w, h, chan, rad, sigma);
    w -= (rad * 2);
    h -= (rad * 2);
    Loader::freePixels(outputPixels);
    Loader::freePixels(pixels);
    pixels = nPix;
}

void Image::loadTexture(int textureSlot)
{
    // Seperate this into multiple functions
    Image::textureSlot = textureSlot;

    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
    Loader::freePixels(pixels);
    pixels = nullptr;

    glActiveTexture(GL_TEXTURE0 + textureSlot);
    glBindTexture(GL_TEXTURE_2D, texID);

    generateVertex();
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
