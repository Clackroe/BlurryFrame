#include <graphics/window.hpp>


int Window::SCR_WIDTH = 0;
int Window::SCR_HEIGHT = 0;

//Static to avoid use outside of this file
static void errorCallback(int error, const char* description) {
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    Window::setHeight(height);
    Window::setWidth(width);
    glViewport(0, 0, width, height);
}

static void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

Window::Window(int SCR_WIDTH, int SCR_HEIGHT, const char* title){
    
    Window::SCR_WIDTH = SCR_WIDTH;
    Window::SCR_HEIGHT = SCR_HEIGHT;

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
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, title, NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }


    initOpenGL();
    initCallbacks();

}

Window::~Window(){
    glfwTerminate();
}

void Window::frameStart(){

    ::processInput(window);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

void Window::Update(){
    //Ensure there are no errors in OpenGL at frame time.
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << err << std::endl;
    }
}

void Window::frameEnd(){
    
    glfwSwapBuffers(window);
    glfwPollEvents();
}

void Window::initCallbacks(){

    glfwSetErrorCallback(errorCallback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
}

int Window::getWidth(){ return SCR_WIDTH;}
int Window::getHeight(){ return SCR_HEIGHT;}

void Window::setWidth(int w){SCR_WIDTH = w;}
void Window::setHeight(int h){SCR_HEIGHT= h;}


void Window::initOpenGL(){
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif


    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        exit(-1);
    }

    //Important for texture packing. By Default it expects textures to have a 
    //padding in their data, which results in slanted textures if not correctly 
    //done. This ensures that it knows to expect textures fully packed.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 

    std::cout << "OpenGL VS: " << glGetString(GL_VERSION) << std::endl;

}


//TODO: Implement
void Window::processInput(){}


