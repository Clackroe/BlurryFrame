#include "image/image.hpp"
#include <management/slide_manager.hpp>
#include <unistd.h>

int seenBuffer = 4;

SlideManager::SlideManager(int size, int timeToSwitch)
{
    timer = BlurTimer();
    timerLength = timeToSwitch;

    shouldClose = false;
    buffSize = size;
    loadQueue = new QueueItem[size];
    imageToRender = nullptr;
    currentImageIndex = 0;
    loadQueue[currentImageIndex].viewing = true;
}

SlideManager::~SlideManager()
{
    delete[] images;
}
bool SlideManager::shouldLoadImage(int index)
{
    return ((currentImageIndex - index + buffSize) % buffSize > seenBuffer
        && !loadQueue[index].viewing);
}

void SlideManager::loadImage() // Called every tick
{
    for (int i = 0; i < buffSize; i++) {
        if (shouldLoadImage(i)) {
            // Load Image i
            // if (loadQueue[i].image == nullptr) {
            //     std::cout << "Constructing new image" << std::endl;
            //     loadQueue[i].image = new Image();
            // }

            loadQueue[i].image->deconstruct();

            loadQueue[i].seen = false;
            loadQueue[i].loaded = true;
        }
    }
}

void SlideManager::switchImage() // Called when timer is elapsed
{
    loadQueue[currentImageIndex].seen = true;
    loadQueue[currentImageIndex].viewing = false;

    imageToRender = &images[currentImageIndex];
    currentImageIndex = (currentImageIndex + 1) % buffSize;
    loadQueue[currentImageIndex].viewing = true;
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
