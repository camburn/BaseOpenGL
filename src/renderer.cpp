#include "renderer.hpp"


enum GLFW_ERRORS {
    GLXBadFBConfig = 65543
};


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
}

void glfw_error_callback(int error, const char* description) {
    std::cerr << "ERROR:: GLFW: " << error << ": " << description << std::endl;

    if (error == GLXBadFBConfig) {
        std::cerr << "WARNING:: OpenGL Core profile (3.3) not supported" << std::endl;
    }
}

void GraphicsOpenGL::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
}

void GraphicsOpenGL::init() {
    glfwSetErrorCallback(glfw_error_callback);
    
    if (!glfwInit()) {
        std::cerr << "WARNING:: Failed to initialise GLFW" << std::endl;
        std::exit(1);
    }

    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    #ifdef GL_CORE_PROFILE
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    //Disable legacy OpenGL
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 
    #endif
   
    //Debugging
    std::cout << "INFO:: OpenGL debugging on" << std::endl;
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
}

bool GraphicsOpenGL::create_window(int width, int height) {
    init();

    window = glfwCreateWindow(width, height, "Aware", NULL, NULL);
    if(window == NULL) {
        std::cout << "WARN:: Failed to open GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);

    #ifdef GL_CORE_PROFILE
    if (gl3wInit()) {
        std::cerr << "ERROR:: Failed to initialise GL3W" << std::endl;
        glfwTerminate();
        std::exit(1);
    }
    if (!gl3wIsSupported(3, 3)) {
        std::cerr << "ERROR:: OpenGL 3.3 not supported\n" << std::endl;
        glfwTerminate();
        std::exit(1);
    }
    #else
    if (glewInit() != GLEW_OK) {
        std::cerr << "ERROR:: GLEW failed to initialise\n" << std::endl;
        glfwTerminate();
        std::exit(1);
    }
    #endif

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetScrollCallback(window, scroll_callback);
    
    glfwGetWindowSize(window, &width, &height);
    std::cout << "Window width: " << width << " height: " << height << std::endl;

    return true;
}

void GraphicsOpenGL::new_frame() {
    glfwPollEvents();

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        running = false;

    interface::new_frame();

    glfwMakeContextCurrent(window);
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
    glClear(GL_COLOR_BUFFER_BIT);
    // Custom Renders


    // IMGUI Interface
    interface::draw();
    glfwSwapBuffers(window);
}

bool GraphicsOpenGL::start() {
    if (!create_window(width, height)) {
        return false;
    }
    interface::start(window);

    return true;
}

void GraphicsOpenGL::exit() {
    interface::exit();
    glfwDestroyWindow(window);
    glfwTerminate();
}

