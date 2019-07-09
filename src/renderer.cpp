#include "renderer.hpp"


Arcball arcball(1920.0f, 1080.0f, 0.1f, true, true);


enum GLFW_ERRORS {
    GLXBadFBConfig = 65543
};




void glfw_error_callback(int error, const char* description) {
    std::cerr << "ERROR:: GLFW: " << error << ": " << description << std::endl;

    if (error == GLXBadFBConfig) {
        std::cerr << "WARNING:: OpenGL Core profile (3.3) not supported" << std::endl;
    }
}

void GraphicsOpenGL::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    arcball.mouseButtonCallback(window, button, action, mods);
}

void GraphicsOpenGL::mouse_cursor_callback(GLFWwindow* window, double xpos, double ypos) {
    arcball.cursorCallback(window, xpos, ypos);
}

void GraphicsOpenGL::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
}

void GraphicsOpenGL::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
}

void GraphicsOpenGL::resize_callback(GLFWwindow* window, int newWidth, int newHeight) {
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
    // Register Key callbacks
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, mouse_cursor_callback);
    glfwSetWindowSizeCallback(window, resize_callback);
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
    glUseProgram(program);
    glFrontFace(GL_CCW);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-5.0f, 0.0f, -5.0f));
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::lookAt(
        glm::vec3(5.0f, 5.0f, 5.0f), // Camera Position
        glm::vec3(0.0f, 0.0f, 0.0f), // Looking at
        glm::vec3(0.0f, 1.0f, 0.0f)  // Up
    );
    view = view * arcball.createViewRotationMatrix();
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(90, 1920/1080, 1, 100);
    
    glm::vec4 color = glm::vec4(1.0f);

    GLuint loc_model = glGetUniformLocation(program, "model");
    GLuint loc_view = glGetUniformLocation(program, "view");
    GLuint loc_proj = glGetUniformLocation(program, "projection");
    GLuint loc_color = glGetUniformLocation(program, "u_color");
    glUniformMatrix4fv(loc_model, 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(loc_view, 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(loc_proj, 1, GL_FALSE, &projection[0][0]);
    glUniform4f(loc_color, color.x, color.y, color.z, color.w);

    terrain->draw();

    // IMGUI Interface
    interface::draw();
    glfwSwapBuffers(window);
}

bool GraphicsOpenGL::start() {
    if (!create_window(width, height)) {
        return false;
    }
    interface::start(window);

  

    program = BuildGlProgram("./shaders/vertex.glsl", "./shaders/fragment.glsl");
    glUseProgram(program);
    terrain = new Terrain{10, 10};

    return true;
}

void GraphicsOpenGL::exit() {
    interface::exit();
    glfwDestroyWindow(window);
    glfwTerminate();
}

