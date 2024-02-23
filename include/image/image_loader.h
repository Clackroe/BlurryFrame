#ifndef IMAGE_LOADER_H
#define  IMAGE_LOADER_H

#include "core.h"
#include "../STB/stb_image.h"
#include "../STB/stb_image_write.h"

namespace Loader {

    unsigned char *loadImage(const char* path, int *width, int *height, int *channels);
    void freePixels(unsigned char* pixels);

}


#endif
