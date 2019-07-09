#include "renderer.hpp"
#include "terrain.hpp"



int main(int argc, char *argv[]) {

    std::cout << "Welcome to program" << std::endl;
    Graphics *graphics = new GraphicsOpenGL {};
    graphics->start();

    while (graphics->running) {
        graphics->new_frame();
    };
    graphics->exit();
}
