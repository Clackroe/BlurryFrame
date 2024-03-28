
#ifndef SLIDE_MANAGER_HPP
#define SLIDE_MANAGER_HPP
#include <image/image.hpp>
#include <thread>
#include <time.hpp>
#include <vector>

struct QueueItem {
    bool seen = false;
    int index = 0;
};

class SlideManager {

public:
    SlideManager(int size, int timeToSwitch);
    ~SlideManager();

    void run();
    void close();

    void loadImage();
    void switchImage();
    void updateQueue();

    int buffSize;
    bool shouldClose;
    bool running;
    BlurTimer timer;
    int timerLength;

    int currentImageIndex;

    Image* images;
    Image* imageToRender;

    std::vector<QueueItem> loadQueue;

    // Debug Vars
    int timeCount = 0;

private:
    void loop();
};

#endif
