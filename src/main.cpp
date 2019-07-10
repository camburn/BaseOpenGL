#include "renderer.hpp"
#include "terrain.hpp"

#include "erode.hpp"
#include "FastNoise.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

int main() {
    std::cout << "Generating Terrain" << std::endl;
    /*
    int size = 512;
    
    calc_brush_indices(size, 3);

    const int seed = 1337;
    const int octaves = 1;

    std::cout << "Generate Noise Data" << std::endl;
    FastNoise big_noise;
    big_noise.SetNoiseType(FastNoise::SimplexFractal);
    big_noise.SetInterp(FastNoise::Quintic);
    big_noise.SetFrequency(0.003f);
    big_noise.SetSeed(seed);
    big_noise.SetFractalGain(0.5f);
    big_noise.SetFractalLacunarity(2.0f);
    big_noise.SetFractalOctaves(octaves);

    FastNoise small_noise;
    small_noise.SetNoiseType(FastNoise::SimplexFractal);
    small_noise.SetInterp(FastNoise::Quintic);
    small_noise.SetFrequency(0.02f);
    small_noise.SetSeed(seed);
    small_noise.SetFractalGain(0.3f);
    small_noise.SetFractalLacunarity(2.0f);
    small_noise.SetFractalOctaves(8);
    
    glm::vec3 *data = new glm::vec3[size * size];

    for (int y = 0; y < size; ++y)
    {
        for (int x = 0; x < size; ++x)
        {
            float sample_big = big_noise.GetNoise(x, y);
            float sample_small = small_noise.GetNoise(x, y);
            float scaled_big = (sample_big + 1.0f) / 2;
            float scaled_small = (sample_small + 1.0f) / 2;
            float height = (scaled_big * 0.9) + (scaled_small * 0.1);
            data[y * size + x] = glm::vec3{(float)x, height, (float)y};
        }
    }

    erode(data, size, 100000);
    */

    GraphicsOpenGL *graphics = new GraphicsOpenGL {};
  
    graphics->start();
    const int size = 512;
    graphics->set_terrain_data(size);

    while (graphics->running) {
        graphics->new_frame();
    };
    graphics->exit();
}
