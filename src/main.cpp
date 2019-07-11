#include "renderer.hpp"
#include "terrain.hpp"

#include "erode.hpp"
#include "FastNoise.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

int main() {
    std::cout << "Generating Terrain" << std::endl;

    GraphicsOpenGL *graphics = new GraphicsOpenGL {};
  
    graphics->start();
    const int size = 512;
    graphics->set_terrain_data(size);

    while (graphics->running) {
        graphics->new_frame();
    };
    graphics->exit();
}
