#include "glm/ext/scalar_constants.hpp"
#include "image/image_loader.h"
#include <image/image.hpp>
#include "graphics/window.hpp"
#include <iostream>
#include <ostream>
#include <vector>

Image::Image(const char* path){
    pixels = Loader::loadImage(path, &w, &h, &chan);
}
Image::~Image(){
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}


static std::vector<float> genGaussianKernal(int radius,  float sigma){
    std::vector<float> kernal;
    float sum = 0.0f;
    for (int i = -radius; i <= radius; ++i){
        for (int j = -radius; j <= radius; ++j){
            int x = i; 
            int y = j;
            float v = x * x + y * y;
            float weight = exp(-v / (2 * sigma * sigma)) / (2 * glm::pi<float>() * sigma * sigma);
            kernal.push_back(weight);
            // std::cout << weight << " ";
            sum += weight;
        }
        // std::cout << std::endl;
    }
    for (float &val: kernal){
        val /= sum;
    }

    return kernal;
}

static void applyGaussianFilter(const unsigned char* inputPixels, unsigned char* outputPixels,
                                int width, int height, int radius, float sigma){
            std::vector<float> kernel = genGaussianKernal(radius, sigma);
            float sumR = 0.0f, sumG = 0.0f, sumB = 0.0f;
            for (int y = 2; y < height - 2; ++y) {
                for (int x = 2; x < width - 2; ++x) {
                    // Compute the weighted sum of pixel values using the Gaussian kernel
                    float sum = 0.0f;
                    for (int ky = -radius; ky <= radius; ++ky) {
                        for (int kx = -radius; kx <= radius; ++kx) {
                            int kernelIndex = (ky + radius) * (radius*2+1) + (kx + radius );
                            int pixelIndex = (y + ky) * width * 3 + (x + kx) * 3;
                            // printf("%i", (int)inputPixels[pixelIndex]);
                            sumR += (int)inputPixels[pixelIndex] *     kernel[kernelIndex];
                            sumG += (int)inputPixels[pixelIndex + 1] * kernel[kernelIndex];
                            sumB += (int)inputPixels[pixelIndex + 2] * kernel[kernelIndex];
                        }
                    }

                // Store the result in the new pixel buffer
                int newPixelIndex = y * width + x;
                outputPixels[newPixelIndex] = glm::clamp((int)sumR, 0, 255); // Normalize by the sum of the kernel
                outputPixels[newPixelIndex + 1] = glm::clamp((int)sumG, 0, 255);
                outputPixels[newPixelIndex + 2] = glm::clamp((int)sumB, 0, 255);
            }
    }
}


void Image::blur(){
    unsigned char *outputPixels = new unsigned char[w * h* 3];
    applyGaussianFilter(pixels, outputPixels, w, h, 2, 0.2f);
    pixels = outputPixels;
}

void Image::render(){
    //TODO: Rethink the layout of this entire class. I don't like the API as 
    //it stands.
    glActiveTexture(GL_TEXTURE0 + textureSlot);
    glBindTexture(GL_TEXTURE_2D, texID);

    glBindVertexArray(VAO); 

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

}

void Image::loadTexture(int textureSlot){

    //TODO: Rethink how this is laid out. We should probably keep the pixels on
    //hand so that we can do image processing, and also the texture slot shouldnt
    //be set here ideally.

    //Apparently windows sucks at predicting what you need....

    //Linux OG: textureSlot = textureSlot
    //Windows though I wanted to set the parameter to itself...

    Image::textureSlot = textureSlot;

    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
    Loader::freePixels(pixels);

    glActiveTexture(GL_TEXTURE0 + textureSlot);
    glBindTexture(GL_TEXTURE_2D, texID);

    generateVertex();
    
}

void Image::generateVertex(){


    // float imageAspectRatio = static_cast<float>(w) / static_cast<float>(h);
    // float screenAspectRatio = static_cast<float>(
    //         Window::getHeight()) / static_cast<float>(Window::getHeight());

    float scaleFactor = 1.0f;
    // if (h > w) {
    //     scaleFactor = static_cast<float>(Window::getHeight()) / static_cast<float>(h);
    // } else {
    //     scaleFactor = static_cast<float>(Window::getWidth()) / static_cast<float>(w);
    // }

    // Adjust vertex positions to maintain aspect ratio
    float scaledWidth = static_cast<float>(w) * scaleFactor;
    float scaledHeight = static_cast<float>(h) * scaleFactor;

    float halfWidth = scaledWidth / 2.0f;
    float halfHeight = scaledHeight / 2.0f;

    float vertexPos[] = {
        halfWidth,  halfHeight, 0.0f,  // top right
        halfWidth, -halfHeight, 0.0f,  // bottom right
       -halfWidth, -halfHeight, 0.0f,  // bottom left
       -halfWidth,  halfHeight, 0.0f   // top left 
    };

    // Normalize vertex positions to be between 0 and 1
    for (int i = 0; i < 4; ++i) {
        vertexPos[i * 3] /= Window::getWidth();
        vertexPos[i * 3 + 1] /= Window::getHeight();
    }

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

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);


    Vertex vertices[4];
    
    for (int i = 0; i < 4; ++i) {
        int texIndex = i * 2;
        int posIndex = i * 3;
        
        vertices[i].pos.x = vertexPos[posIndex];
        vertices[i].pos.y = vertexPos[posIndex + 1];
        vertices[i].pos.z = vertexPos[posIndex + 2];
        
        vertices[i].texUV.x = textureCoords[texIndex];
        vertices[i].texUV.y = textureCoords[texIndex + 1];
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,  sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    //Unbind VAO
    glBindVertexArray(0); 
}
