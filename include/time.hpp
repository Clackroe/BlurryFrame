#ifndef TIME_HPP
#define TIME_HPP

#include <GLFW/glfw3.h>

class BlurTimer {
public:
    BlurTimer()
    {
        count = 0;
        time = 0.0f;
        targetTime = 0.0f;
        startTime = glfwGetTime();
    }

    void tick()
    {
        float cur = glfwGetTime();
        float del = cur - startTime;
        time += del;
        startTime = cur;
        count++;
    }

    void setTimerSeconds(float targetTimeSeconds)
    {
        targetTime = targetTimeSeconds;
        time = 0.0f;
        count = 0;
    }

    bool isTimerElapsed()
    {
        return time >= targetTime;
    }

    float getElapsedTime()
    {
        return time;
    }

    float getRemainingTime()
    {
        return targetTime - time;
    }

private:
    float time;
    float startTime;
    int count;
    float targetTime;
};

#endif
