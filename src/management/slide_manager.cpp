#include "image/image.hpp"
#include <exception>
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

    //
    //
    //
    //
    //
    //
    //
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
    return (!loadQueue[index].viewing && loadQueue[index].shouldLoad);
}

void SlideManager::loadImage() // Called every tick
{
    for (int i = 0; i < buffSize; i++) {
        if (shouldLoadImage(i)) {

            std::cout << fIndex << std::endl;
            std::cout << files[fIndex] << std::endl;
            loadQueue[i].image->deconstruct();
            loadQueue[i].image->reconstruct(files[fIndex].data());
            loadQueue[i].image->generateVertex();
            loadQueue[i].shouldLoad = false;
            fIndex = (fIndex + 1) % files.size();
        }
    }
}

void SlideManager::switchImage() // Called when timer is elapsed
{
    loadQueue[currentImageIndex].viewing = false;
    loadQueue[(currentImageIndex - seenBuffer + buffSize) % buffSize].shouldLoad = true;
    imageToRender = loadQueue[currentImageIndex].image;
    currentImageIndex = (currentImageIndex + 1) % buffSize;
    loadQueue[currentImageIndex].viewing = true;

    // (currentImageIndex - index + buffSize) % buffSize > seenBuffer

    if (imageToRender == nullptr) {
        std::cout << "Why am I null????" << std::endl;
    }
    std::cout << "Not null, heck yea...." << std::endl;
}

void SlideManager::prev()
{
    int cin = currentImageIndex;
    int prevIn = (currentImageIndex - 1 + buffSize) % buffSize;

    loadQueue[cin].viewing = false;
    loadQueue[cin].shouldLoad = true;
    loadQueue[prevIn].viewing = true;
    loadQueue[prevIn].shouldLoad = false;

    currentImageIndex = prevIn;
    imageToRender = loadQueue[prevIn].image;
}
void SlideManager::next()
{
    int cin = currentImageIndex;
    int prevIn = (currentImageIndex + 1 + buffSize) % buffSize;

    loadQueue[cin].viewing = false;
    loadQueue[cin].shouldLoad = true;
    loadQueue[prevIn].viewing = true;
    loadQueue[prevIn].shouldLoad = false;

    currentImageIndex = prevIn;
    imageToRender = loadQueue[prevIn].image;
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
