#include "image/image_loader.h"


namespace Loader {

    unsigned char *loadImage(const char *path, int &width, int &height, int &channels) {

      stbi_set_flip_vertically_on_load(true);

      unsigned char* data = stbi_load(path, &width, &height, &channels, 0);
      if (!data) {
          std::cerr << "Load Image Failed: " << path << std::endl;
          return nullptr;
      }
      return data;      
    }

    void freePixels(unsigned char* pixels){
        if (pixels){
            stbi_image_free(pixels);
            std::cout << "Data Freed" << std::endl;
        }
        else {
            std::cerr << "No data to free." << std::endl;
        }


    }

}
