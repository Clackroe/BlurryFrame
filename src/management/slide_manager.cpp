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
    currentImageIndex = 0;
    imageToRender = nullptr;

    // TODO: Find a different way to do this
    std::string path = "content/";
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        if (entry.path().extension() == ".jpg" || entry.path().extension() == ".png" || entry.path().extension() == ".jpeg" || entry.path().extension() == ".JPG") {
            files.push_back(entry.path().string());
        }
    }

    // End TODO
    //
    for (int i = 0; i < buffSize; i++) {
        if (shouldLoadImage(i)) {

            loadQueue[i].burImage.deconstruct();
            loadQueue[i].burImage.reconstruct(files[fIndex].data());
            loadQueue[i].burImage.blur(20);
            loadQueue[i].burImage.generateVertex();

            loadQueue[i].image.deconstruct();
            loadQueue[i].image.reconstruct(files[fIndex].data());
            loadQueue[i].image.generateVertex();
            loadQueue[i].name = files[fIndex];
            loadQueue[i].shouldLoad = false;
            fIndex = (fIndex + 1) % files.size();
        }
    }
    imageToRender = &loadQueue[currentImageIndex];
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
        if ((currentImageIndex - i + buffSize) % buffSize <= seenBuffer) {
            loadQueue[i].shouldLoad = false;
        }
        if (shouldLoadImage(i)) {

            // GlCleanupItem imageCleanup;
            // imageCleanup.EBO = &loadQueue[i].image.EBO;
            // imageCleanup.VAO = &loadQueue[i].image.VAO;
            // imageCleanup.VBO = &loadQueue[i].image.VBO;
            // GlCleanupItem blurCleanup;
            // blurCleanup.EBO = &loadQueue[i].burImage.EBO;
            // blurCleanup.VAO = &loadQueue[i].burImage.VAO;
            // blurCleanup.VBO = &loadQueue[i].burImage.VBO;

            // glCleanupBuffer.push_back(imageCleanup);
            // glCleanupBuffer.push_back(blurCleanup);

            loadQueue[i].image.deconstruct();
            loadQueue[i].image.reconstruct(files[fIndex].data());
            loadQueue[i].image.generateVertex();

            loadQueue[i].burImage.deconstruct();
            loadQueue[i].burImage.reconstruct(files[fIndex].data());
            loadQueue[i].burImage.blur(20);
            loadQueue[i].burImage.generateVertex();

            loadQueue[i].name = files[fIndex];
            loadQueue[i].shouldLoad = false;
            fIndex = (fIndex + 1) % files.size();
        }
    }
}

void SlideManager::switchImage() // Called when timer is elapsed
{
    loadQueue[currentImageIndex].viewing = false;
    loadQueue[(currentImageIndex - seenBuffer + buffSize) % buffSize].shouldLoad = true;
    imageToRender = &loadQueue[currentImageIndex];
    currentImageIndex = (currentImageIndex + 1) % buffSize;
    loadQueue[currentImageIndex].viewing = true;
}

void SlideManager::prev()
{
    int prevIn = (currentImageIndex - 1 + buffSize) % buffSize;

    loadQueue[currentImageIndex].viewing = false;
    loadQueue[currentImageIndex].shouldLoad = true;
    loadQueue[prevIn].viewing = true;
    loadQueue[prevIn].shouldLoad = false;

    loadQueue[(currentImageIndex - seenBuffer + buffSize) % buffSize].shouldLoad = true;
    currentImageIndex = prevIn;
    imageToRender = &loadQueue[prevIn];
}
void SlideManager::next()
{
    int prevIn = (currentImageIndex + 1 + buffSize) % buffSize;

    loadQueue[currentImageIndex].viewing = false;
    loadQueue[currentImageIndex].shouldLoad = true;
    loadQueue[prevIn].viewing = true;
    loadQueue[prevIn].shouldLoad = false;

    loadQueue[(currentImageIndex - seenBuffer + buffSize) % buffSize].shouldLoad = true;
    currentImageIndex = prevIn;
    imageToRender = &loadQueue[prevIn];
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
