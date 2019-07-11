#include <string>
#include "imgui.h"

#include "imgui_impl_glfw.h"
#ifdef GL_CORE_PROFILE
#define IMGUI_IMPL_OPENGL_LOADER_GL3W
#include "imgui_impl_opengl3.h"
#else
#define IMGUI_IMPL_OPENGL_LOADER_GLEW
#include "imgui_impl_opengl2.h"
#endif

#include "terrain.hpp"

namespace interface {

void start(GLFWwindow* windows);

void new_frame();

void draw();

void exit();

bool controls_io();

void set_terrain(Terrain *terrain);

}

