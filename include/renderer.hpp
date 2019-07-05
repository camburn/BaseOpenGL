#ifndef GL_GRAPHICS_HPP
#define GL_GRAPHICS_HPP
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <atomic>
#ifdef GL_CORE_PROFILE
#include <GL/gl3w.h>
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "graphics.hpp"
#include "interface.hpp"

class GraphicsOpenGL: public Graphics {
private:
    GLFWwindow *window;

    int width = 1920;
    int height = 1080;

    bool create_window(int width, int height);
    void init();

public:
    void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    void new_frame();
    bool start();
    void exit();
};

#endif
