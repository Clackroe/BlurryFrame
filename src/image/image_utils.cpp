#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "image/image_utils.hpp"
#include <memory>

namespace Loader {

unsigned char* loadImage(const char* path, int* width, int* height, int* channels)
{

    stbi_set_flip_vertically_on_load(true);

    unsigned char* data = stbi_load(path, width, height, channels, 0);
    if (!data) {
        std::cerr << "Load Image Failed: " << path << std::endl;
        return nullptr;
    }
    return data;
}

void freePixels(unsigned char* pixels)
{
    if (pixels != nullptr && pixels) {
        stbi_image_free(pixels);
    } else {
        std::cerr << "No Data To Free" << std::endl;
    }
}

}

namespace Blur {

// 1D Gaussian to take advantage of seperable filters.
std::vector<float> genGaussianKernal1D(int radius, float sigma)
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

void downsample_image(unsigned char* image, unsigned char* output, int* width, int* height, int newWidth, int newHeight, int channels)
{
    stbir_resize_uint8_linear(image, *width, *height, 0, output, newWidth, newHeight, 0, (stbir_pixel_layout)channels);
    *width = newWidth;
    *height = newHeight;
}

// void applyGaussianFilter(const unsigned char* inputPixels, unsigned char* outputPixels,
//     int width, int height, int channels, int radius, float sigma)
// {
//
//     std::vector<float> kernel = genGaussianKernal1D(radius, sigma);
//
//     // Apply in the horizontal axis
//     for (int y = radius; y < height - radius; y += 1) {
//         for (int x = radius; x < width - radius; x += 1) {
//             int pixelIndex = (y * width * channels) + x * channels;
//             float sumR = 0.0f;
//             float sumG = 0.0f;
//             float sumB = 0.0f;
//
//             float sumWeights = 0.0f;
//
//             int kIndex = 0;
//             for (int kx = -radius; kx <= radius; kx++) {
//                 int kPIndex = (y * width * channels) + (x + kx) * channels;
//                 sumR += inputPixels[kPIndex + 0] * kernel[kIndex];
//                 sumG += inputPixels[kPIndex + 1] * kernel[kIndex];
//                 sumB += inputPixels[kPIndex + 2] * kernel[kIndex];
//                 sumWeights += kernel[kIndex];
//                 kIndex++;
//             }
//
//             pixelIndex = ((y - radius) * (width - radius * 2) * channels) + (x - radius) * channels;
//             outputPixels[pixelIndex + 0] = glm::clamp((sumR / sumWeights), 0.0f, 255.0f);
//             outputPixels[pixelIndex + 1] = glm::clamp((sumG / sumWeights), 0.0f, 255.0f);
//             outputPixels[pixelIndex + 2] = glm::clamp((sumB / sumWeights), 0.0f, 255.0f);
//         }
//     }
//
//     // Apply kernel in the vertical.
//     for (int y = radius; y < height - radius; y += 1) {
//         for (int x = radius; x < width - radius; x += 1) {
//             int pixelIndex = (y * width * channels) + x * channels;
//             float sumR = 0.0f;
//             float sumG = 0.0f;
//             float sumB = 0.0f;
//
//             float sumWeights = 0.0f;
//
//             int kIndex = 0;
//             for (int ky = -radius; ky <= radius; ky++) {
//                 int kPIndex = ((y + ky) * width * channels) + x * channels;
//                 sumR += outputPixels[kPIndex + 0] * kernel[kIndex];
//                 sumG += outputPixels[kPIndex + 1] * kernel[kIndex];
//                 sumB += outputPixels[kPIndex + 2] * kernel[kIndex];
//                 sumWeights += kernel[kIndex];
//                 kIndex++;
//             }
//
//             pixelIndex = ((y - radius) * (width - (radius * 2)) * channels) + (x - radius) * channels;
//             outputPixels[pixelIndex + 0] = glm::clamp((sumR / sumWeights), 0.0f, 255.0f);
//             outputPixels[pixelIndex + 1] = glm::clamp((sumG / sumWeights), 0.0f, 255.0f);
//             outputPixels[pixelIndex + 2] = glm::clamp((sumB / sumWeights), 0.0f, 255.0f);
//         }
//     }
// }
//

void applyGaussianFilter(const unsigned char* inputPixels, unsigned char* outputPixels,
    int width, int height, int channels, int radius, float sigma)
{
    std::vector<float> kernel = genGaussianKernal1D(radius, sigma);

    // Extend image boundaries
    int extendedWidth = width + 2 * radius;
    int extendedHeight = height + 2 * radius;

    unsigned char* extendedInputPixels = new unsigned char[extendedWidth * extendedHeight * channels];

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int srcIndex = (y * width) + x * channels;
            int destIndex = ((y + radius) * extendedWidth + (x + radius)) * channels;
            // int destIndex = ((y + radius) * (extendedWidth - radius) + (x - radius)) * channels;
            memcpy(&extendedInputPixels[destIndex], &inputPixels[srcIndex], channels);
        }
    }

    // Apply in the horizontal axis
    for (int y = radius; y < extendedHeight - radius; y++) {
        for (int x = radius; x < extendedWidth - radius; x++) {
            int pixelIndex = (y * extendedWidth * channels) + x * channels;
            float sumR = 0.0f;
            float sumG = 0.0f;
            float sumB = 0.0f;
            float sumWeights = 0.0f;

            int kIndex = 0;
            for (int kx = -radius; kx <= radius; kx++) {
                int kPIndex = (y * extendedWidth * channels) + (x + kx) * channels;
                if (kPIndex >= 0 && kPIndex < extendedWidth * extendedHeight * channels) {
                    sumR += extendedInputPixels[kPIndex + 0] * kernel[kIndex];
                    sumG += extendedInputPixels[kPIndex + 1] * kernel[kIndex];
                    sumB += extendedInputPixels[kPIndex + 2] * kernel[kIndex];
                    sumWeights += kernel[kIndex];
                }
                kIndex++;
            }

            pixelIndex = ((y - radius) * (extendedWidth - radius * 2) * channels) + (x - radius) * channels;
            if (pixelIndex >= 0 && pixelIndex + channels * 2 <= extendedWidth * extendedHeight * channels) {
                outputPixels[pixelIndex + 0] = glm::clamp((sumR / sumWeights), 0.0f, 255.0f);
                outputPixels[pixelIndex + 1] = glm::clamp((sumG / sumWeights), 0.0f, 255.0f);
                outputPixels[pixelIndex + 2] = glm::clamp((sumB / sumWeights), 0.0f, 255.0f);
            }
        }
    }

    // Apply kernel in the vertical.
    for (int y = radius; y < extendedHeight - radius; y++) {
        for (int x = radius; x < extendedWidth - radius; x++) {
            int pixelIndex = (y * extendedWidth * channels) + x * channels;
            float sumR = 0.0f;
            float sumG = 0.0f;
            float sumB = 0.0f;
            float sumWeights = 0.0f;

            int kIndex = 0;
            for (int ky = -radius; ky <= radius; ky++) {
                int kPIndex = ((y + ky) * extendedWidth * channels) + x * channels;
                if (kPIndex >= 0 && kPIndex < extendedWidth * extendedHeight * channels) {
                    sumR += outputPixels[kPIndex + 0] * kernel[kIndex];
                    sumG += outputPixels[kPIndex + 1] * kernel[kIndex];
                    sumB += outputPixels[kPIndex + 2] * kernel[kIndex];
                    sumWeights += kernel[kIndex];
                }
                kIndex++;
            }

            pixelIndex = ((y - radius) * (extendedWidth - (radius * 2)) * channels) + (x - radius) * channels;
            if (pixelIndex >= 0 && pixelIndex + channels * 2 <= extendedWidth * extendedHeight * channels) {
                outputPixels[pixelIndex + 0] = glm::clamp((sumR / sumWeights), 0.0f, 255.0f);
                outputPixels[pixelIndex + 1] = glm::clamp((sumG / sumWeights), 0.0f, 255.0f);
                outputPixels[pixelIndex + 2] = glm::clamp((sumB / sumWeights), 0.0f, 255.0f);
            }
        }
    }

    // Loader::freePixels(extendedInputPixels);
}
}
