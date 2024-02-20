#include <GLFW/glfw3.h>
#include <iostream>

// Error callback function
void errorCallback(int error, const char* description) {
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

int main() {
    // Set the error callback function
    glfwSetErrorCallback(errorCallback);

    // Initialize GLFW
    if (!glfwInit()) {
        // Initialization failed
        return -1;
    }

    // Create a window or perform other GLFW operations here

    // Terminate GLFW
    glfwTerminate();

    return 0;
}
