#include <iostream>
#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

using namespace std;



    int width, height, channels;
    unsigned char *img = stbi_load(path, &width, &height, &channels, 0);
    if (img == NULL) {
        cout << "Error loading image" << endl;
        exit(1);
    }
    printf("Loaded image with %dpx, %dpx, %d\n", width, height, channels);

    // this just writes the png to the current directory
    // stbi_write_png("test1.png", width, height, channels, img, width * channels);
    // stbi_write_jpg(path, width, height, channels, img, 100);

    // load img to gpu (next step)

    stbi_image_free(img);
}

