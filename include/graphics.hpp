#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP
#include <string>
#include <vector>


class Graphics {
public:
    bool running = true;
    virtual bool start() = 0;
    virtual void exit() = 0;
    virtual void new_frame() = 0;
    virtual ~Graphics() {};
};

#endif
