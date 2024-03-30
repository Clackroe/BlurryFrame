
#ifndef SLIDE_MANAGER_HPP
#define SLIDE_MANAGER_HPP
#include <image/image.hpp>
#include <thread>
#include <time.hpp>

struct QueueItem {
    bool seen = false;
    bool viewing = false;
    bool loaded = false;
    Image* image = new Image();
};

class SlideManager {

public:
    SlideManager(int size, int timeToSwitch);
    ~SlideManager();

    void run();
    void close();

    void loadImage();
    void switchImage();
    bool shouldLoadImage(int index);

    int buffSize;
    bool shouldClose;
    bool running;
    BlurTimer timer;
    int timerLength;

    int currentImageIndex;

    Image* imageToRender;

    std::thread main;

    QueueItem* loadQueue;

    // Debug Vars
    int timeCount = 0;

private:
    void loop();
};

#endif
