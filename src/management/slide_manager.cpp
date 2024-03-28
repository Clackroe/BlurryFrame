#include <cstdio>
#include <exception>
#include <management/slide_manager.hpp>
#include <unistd.h>

SlideManager::SlideManager(int size, int timeToSwitch)
{
    timer = BlurTimer();
    timerLength = timeToSwitch;

    shouldClose = false;
    buffSize = size;
    images = new Image[size];
    imageToRender = nullptr;
    currentImageIndex = 0;
    loadQueue = std::vector<QueueItem>(buffSize);
}

SlideManager::~SlideManager()
{
    delete[] images;
}

void SlideManager::loadImage()
{
}

void SlideManager::switchImage()
{
    imageToRender = &images[currentImageIndex];
    currentImageIndex = (currentImageIndex + 1) % buffSize;
}

void SlideManager::updateQueue()
{
}

void SlideManager::run()
{
    if (running == true) {
        return;
    }
    timer.setTimerSeconds(timerLength);
    std::thread main(&SlideManager::loop, this);
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
