
#ifndef SLIDE_MANAGER_HPP
#define SLIDE_MANAGER_HPP
#include <image/image.hpp>
#include <thread>
#include <time.hpp>

struct QueueItem {
    bool viewing = false;
    bool shouldLoad = true;
    std::string name = "none";
    Image image = Image();
    Image burImage = Image();
};

struct GlCleanupItem {

    unsigned int *VBO, *VAO, *EBO;
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

    void prev();
    void next();

    std::vector<GlCleanupItem> glCleanupBuffer = std::vector<GlCleanupItem>();

    int buffSize;
    bool shouldClose;
    bool running;
    BlurTimer timer;
    int timerLength;

    int currentImageIndex;

    QueueItem* imageToRender;

    std::thread main;

    QueueItem* loadQueue;

    // Debug Vars
    int timeCount = 0;

private:
    void loop();
};

#endif
