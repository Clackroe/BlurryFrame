#include <graphics/window.hpp>

int Window::SCR_WIDTH = 0;
int Window::SCR_HEIGHT = 0;

// Static to avoid use outside of this file
static void errorCallback(int error, const char* description)
{
    if (error == 65537) {
        return;
    }
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    Window::setHeight(height);
    Window::setWidth(width);
    glViewport(0, 0, width, height);
}

static void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

Window::Window(const char* title)
{

    glfwSetErrorCallback(errorCallback);

    if (!glfwInit()) {
        // Initialization failed
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(-1);
    }
    // define openGL version and profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__ // MacOS is so silly
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    Window::SCR_WIDTH = mode->width;
    Window::SCR_HEIGHT = mode->height;
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, title, glfwGetPrimaryMonitor(), NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }

    initOpenGL();
    initCallbacks();
}

Window::~Window()
{
    glfwTerminate();
}

void Window::frameStart()
{

    ::processInput(window);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::Update()
{
    // Ensure there are no errors in OpenGL at frame time.
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        // TODO: Uncomment if need errors
        // std::cerr << err << std::endl;
    }
}

void Window::frameEnd()
{

    glfwSwapBuffers(window);
    glfwPollEvents();
}

void Window::initCallbacks()
{

    glfwSetErrorCallback(errorCallback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
}

int Window::getWidth() { return SCR_WIDTH; }
int Window::getHeight() { return SCR_HEIGHT; }

void Window::setWidth(int w) { SCR_WIDTH = w; }
void Window::setHeight(int h) { SCR_HEIGHT = h; }

void Window::initOpenGL()
{
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        exit(-1);
    }

    // Important for texture packing. By Default it expects textures to have a
    // padding in their data, which results in slanted textures if not correctly
    // done. This ensures that it knows to expect textures fully packed.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

// TODO: Implement
void Window::processInput() { }
