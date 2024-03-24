#ifndef IMAGE_LOADER_H
#define IMAGE_LOADER_H

#include "../STB/stb_image.hpp"
#include "../STB/stb_image_write.hpp"
#include "core.h"

namespace Loader {

unsigned char* loadImage(const char* path, int* width, int* height, int* channels);
void freePixels(unsigned char* pixels);

}

#endif
