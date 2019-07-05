#include "renderer.hpp"
#include "terrain.hpp"



int main(int argc, char *argv[]) {

    std::cout << "Welcome to program" << std::endl;
    Graphics *graphics = new GraphicsOpenGL {};
    graphics->start();

    Terrain *terrain = new Terrain{10, 10};

    while (graphics->running) {
        graphics->new_frame();
    };
    graphics->exit();
}
