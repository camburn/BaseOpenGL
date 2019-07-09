#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP
#include <string>
#include <vector>

#ifdef GL_CORE_PROFILE
#include <GL/gl3w.h>
#else
#include <GL/glew.h>
#endif

class Graphics {
public:
    bool running = true;
    virtual bool start() = 0;
    virtual void exit() = 0;
    virtual void new_frame() = 0;
    virtual ~Graphics() {};
};

#endif
