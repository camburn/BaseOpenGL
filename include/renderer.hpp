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
#include "terrain.hpp"
#include "arcball.hpp"


class GraphicsOpenGL: public Graphics {
private:
    GLFWwindow *window;

    int width = 1920;
    int height = 1080;

    bool create_window(int width, int height);
    void init();

    Terrain* terrain;

    GLuint program;

public:
	
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void mouse_cursor_callback(GLFWwindow* window, double xpos, double ypos);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void resize_callback(GLFWwindow* window, int newWidth, int newHeight);

    void new_frame();
    bool start();
    void exit();
};

#endif
