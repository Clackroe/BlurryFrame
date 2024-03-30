#include "image/image.hpp"
#include <filesystem>
#include <management/slide_manager.hpp>
#include <unistd.h>
#include <vector>

int seenBuffer = 4;

// SUPPPPER JANK currently
static std::vector<std::string> files;
static int fIndex = 0;

SlideManager::SlideManager(int size, int timeToSwitch)
{
    timer = BlurTimer();
    timerLength = timeToSwitch;

    shouldClose = false;
    buffSize = size;
    loadQueue = new QueueItem[size];
    imageToRender = nullptr;
    currentImageIndex = 0;

    std::string path = "content/";
    // get all files that are images
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        if (entry.path().extension() == ".jpg" || entry.path().extension() == ".png" || entry.path().extension() == ".jpeg" || entry.path().extension() == ".JPG") {
            files.push_back(entry.path().string());
        }
    }
    // Super jank--- Temp
    delete loadQueue[currentImageIndex].image;
    loadQueue[currentImageIndex].image = new Image(files[fIndex]);
    fIndex = (fIndex + 1) % files.size();
    loadQueue[currentImageIndex].viewing = true;
}

SlideManager::~SlideManager()
{
    delete[] loadQueue;
}
bool SlideManager::shouldLoadImage(int index)
{
    return ((currentImageIndex - index + buffSize) % buffSize > seenBuffer
        && !loadQueue[index].viewing && !loadQueue[index].loaded);
}

void SlideManager::loadImage() // Called every tick
{
    for (int i = 0; i < buffSize; i++) {
        if (shouldLoadImage(i)) {
            // Load Image i

            delete loadQueue[i].image;
            std::cout << fIndex << std::endl;
            std::cout << files[fIndex] << std::endl;
            loadQueue[i].image = new Image(files[fIndex]);
            fIndex = (fIndex + 1) % files.size();
            // Very Jank
            // int in = (currentImageIndex - i + buffSize) % buffSize;
            // loadQueue[currentImageIndex].image->loadTexture(0);

            loadQueue[i].seen = false;
            loadQueue[i].loaded = true;
        }
    }
}

void SlideManager::switchImage() // Called when timer is elapsed
{
    loadQueue[currentImageIndex].seen = true;
    loadQueue[currentImageIndex].viewing = false;
    loadQueue[currentImageIndex].loaded = false;

    imageToRender = loadQueue[currentImageIndex].image;
    currentImageIndex = (currentImageIndex + 1) % buffSize;
    loadQueue[currentImageIndex].viewing = true;

    if (imageToRender == nullptr) {
        std::cout << "Why am I null????" << std::endl;
    }
    std::cout << "Not null, heck yea...." << std::endl;
}

void SlideManager::run()
{
    if (running == true) {
        return;
    }
    timer.setTimerSeconds(timerLength);
    main = std::thread(&SlideManager::loop, this);
    main.detach();
}

void SlideManager::loop()
{
    running = true;
    while (!shouldClose) {
        timer.tick();
        if (timer.isTimerElapsed()) {
            switchImage();
            timeCount++;
            timer.setTimerSeconds(timerLength);
        }
        loadImage();
    }
}

void SlideManager::close()
{
    shouldClose = true;
}
