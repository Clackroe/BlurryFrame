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

void downsample_image(unsigned char* image, unsigned char* output, int* width, int* height, int newWidth, int newHeight, int channels)
{
    stbir_resize_uint8_linear(image, *width, *height, 0, output, newWidth, newHeight, 0, (stbir_pixel_layout)channels);
    *width = newWidth;
    *height = newHeight;
}

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

void applyGaussianFilter(const unsigned char* inputPixels, unsigned char* outputPixels,
    int& width, int& height, int channels, int radius, float sigma, Padding padding)
{

    // Add 0 padding
    int extendedWidth = width + (2 * radius);
    int extendedHeight = height + (2 * radius);
    unsigned char* extendedInputPixels = new unsigned char[extendedWidth * extendedHeight * channels];
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int srcIndex = ((y * width) + x) * channels;
            int destIndex = (((y + radius) * extendedWidth) + (x + radius)) * channels;
            for (int c = 0; c < channels; c++) {
                extendedInputPixels[destIndex + c] = inputPixels[srcIndex + c];
            }
        }
    }

    if (padding != Padding::CLIP) {

        // Mirror padding for top and bottom rows
        for (int y = 0; y < radius; y++) {
            for (int x = 0; x < width; x++) {
                int srcTopIndex = (((radius - y) * width) + x) * channels;
                int srcBottomIndex = (((height - 1 - y) * width) + x) * channels;
                int destTopIndex = ((y * extendedWidth) + (x + radius)) * channels;
                int destBottomIndex = (((extendedHeight - 1 - y) * extendedWidth) + (x + radius)) * channels;
                for (int c = 0; c < channels; c++) {
                    extendedInputPixels[destTopIndex + c] = inputPixels[srcTopIndex + c];
                    extendedInputPixels[destBottomIndex + c] = inputPixels[srcBottomIndex + c];
                }
            }
        }
        // Mirror padding for left and right columns
        for (int y = radius; y < extendedHeight - radius; y++) {
            for (int x = 0; x < radius; x++) {
                int srcLeftIndex = (((y - radius) * width) + (radius + x)) * channels;
                int srcRightIndex = (((y - radius) * width) + (width - 1 - x)) * channels;
                int destLeftIndex = ((y * extendedWidth) + x) * channels;
                int destRightIndex = ((y * extendedWidth) + (extendedWidth - 1 - x)) * channels;
                for (int c = 0; c < channels; c++) {
                    extendedInputPixels[destLeftIndex + c] = inputPixels[srcLeftIndex + c];
                    extendedInputPixels[destRightIndex + c] = inputPixels[srcRightIndex + c];
                }
            }
        }
    }

    std::vector<float> kernel = genGaussianKernal1D(radius, sigma);

    // Vertical
    for (int x = radius; x < extendedWidth - radius; x++) {
        for (int y = radius; y < extendedHeight - radius; y++) {

            int pixelIndex = ((y * extendedWidth) + x) * channels;
            std::vector<float> channelSums(channels, 0.0f);
            float sumOfWeights = 0.0f;

            int kernelIndex = 0;
            for (int ky = -radius; ky <= radius; ky++) {
                int kernelPixelindex = (((y + ky) * extendedWidth) + x) * channels;
                for (int c = 0; c < channelSums.size(); c++) {
                    channelSums[c] += extendedInputPixels[kernelPixelindex + c] * kernel[kernelIndex];
                }
                sumOfWeights += kernel[kernelIndex];
                kernelIndex++;
            }

            for (int c = 0; c < channelSums.size(); c++) {
                extendedInputPixels[pixelIndex + c] = (int)glm::clamp((channelSums[c] / sumOfWeights), 0.0f, 255.0f);
            }
        }
    }

    // Horizontal
    for (int x = radius; x < extendedWidth - radius; x++) {
        for (int y = radius; y < extendedHeight - radius; y++) {

            int pixelIndex = ((y * extendedWidth) + x) * channels;
            std::vector<float> channelSums(channels, 0.0f);
            float sumOfWeights = 0.0f;

            int kernelIndex = 0;
            for (int kx = -radius; kx <= radius; kx++) {
                int kernelPixelindex = ((y * extendedWidth) + (x + kx)) * channels;
                for (int c = 0; c < channelSums.size(); c++) {
                    channelSums[c] += extendedInputPixels[kernelPixelindex + c] * kernel[kernelIndex];
                }
                sumOfWeights += kernel[kernelIndex];
                kernelIndex++;
            }
            for (int c = 0; c < channelSums.size(); c++) {
                extendedInputPixels[pixelIndex + c] = (int)glm::clamp((channelSums[c] / sumOfWeights), 0.0f, 255.0f);
            }
        }
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int srcIndex = (((y + radius) * extendedWidth) + (x + radius)) * channels;
            int destIndex = ((y * width) + x) * channels;
            for (int c = 0; c < channels; c++) {
                outputPixels[destIndex + c] = extendedInputPixels[srcIndex + c];
            }
        }
    }
    delete[] extendedInputPixels;
}
}
