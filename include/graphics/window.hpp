#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

class Window {
public:

    Window(int SCR_WIDTH, int SCR_HEIGHT, const char* title);
    ~Window();

    static int getWidth();
    static int getHeight();

    void frameStart();
    void Update();
    void frameEnd();
    GLFWwindow* window;

private:
    static int SCR_WIDTH, SCR_HEIGHT;
    const char* window_title;
    void initOpenGL();
    void processInput();
    void initCallbacks();


};
