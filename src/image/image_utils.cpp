#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "image/image_utils.hpp"

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

unsigned char* downsample_image(unsigned char* image, int width, int height, int newWidth, int newHeight)
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

void applyGaussianFilter(const unsigned char* inputPixels, unsigned char* outputPixels,
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
}
