#ifndef IMAGE_UTILS_H
#define IMAGE_UTILS_H

#include "../STB/stb_image.hpp"
#include "../STB/stb_image_write.hpp"
#include "core.h"
#include "glm/ext/matrix_transform.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

namespace Loader {

unsigned char* loadImage(const char* path, int* width, int* height, int* channels);
void freePixels(unsigned char* pixels);

}

namespace Blur {

std::vector<float> genGaussianKernal1D(int radius, float sigma);
unsigned char* downsample_image(unsigned char* image, int width, int height, int newWidth, int newHeight);
void applyGaussianFilter(const unsigned char* inputPixels, unsigned char* outputPixels,
    int width, int height, int channels, int radius, float sigma);
}

#endif
