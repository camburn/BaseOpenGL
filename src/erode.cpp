#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <random>

#include "image.hpp"
#include "FastNoise.hpp"


class V3{

public:
    float x;
    float y;
    float z;

    V3(): x(0.0f), y(0.0f), z(0.0f) {}
    V3(float x, float y, float z): x(x), y(y), z(z) {
    }
};

const int EROSION_RADIUS = 3;
const float INITIAL_SPEED = 1.0f;
const float INITIAL_WATER_VOLUME = 1.0f;
const float MAX_DROPLET_LIFETIME = 30.0f;
const float ERODE_SPEED = 0.3f;
const float DEPOSIT_SPEED = 0.3f;
const float EVAPORATE_SPEED = 0.01f;
const float INERTIA = 0.05f;
const float SEDIMENT_CAPACITY_FACTOR = 4.0f;
const float MIN_SEDIMENT_CAPACITY = 0.1f;
const float GRAVITY = 4.0f;

class HeightAndGradient{
public:
    float height;
    float gradient_x;
    float gradient_y;
};


template<size_t width, size_t height>
HeightAndGradient CalculateHeightAndGradient(V3 (&data)[width][height], float pos_x, float pos_y) {
    int coord_x = (int)pos_x;
    int coord_y = (int)pos_y;

    float x = pos_x - coord_x;
    float y = pos_y - coord_y;

    float height_sw = data[coord_x][coord_y].z;
    float height_se = data[coord_x + 1][coord_y].z;
    float height_nw = data[coord_x][coord_y + 1].z;
    float height_ne = data[coord_x + 1][coord_y + 1].z;

    float gradient_x = (height_ne - height_nw) * (1 - y) + (height_se - height_sw) * y;
    float gradient_y = (height_sw - height_nw) * (1 - x) + (height_se - height_ne) * x;

    float cell_height = height_nw * (1 - x) * (1 - y) + height_ne * x * (1 - y) + height_sw * (1 - x) * y + height_se * x * y;

    return HeightAndGradient {cell_height, gradient_x, gradient_y};
}

void InitialiseBrushIndices(int mapsize, int radius) {
    int brush_indices[mapsize][mapsize];
    float brush_weights[mapsize][mapsize];
}

template<size_t width, size_t height>
void erode(V3 (&data)[width][height], int iterations) {

    std::default_random_engine random_source;
    std::uniform_real_distribution<float> random_x(0.0f, (float)width - 1);
    std::uniform_real_distribution<float> random_y(0.0f, (float)height - 1);

    // Create water droplet at random point on map
    for (int iteration = 0; iteration < iterations; iteration++) {
        float pos_x = random_x(random_source);
        float pos_y = random_y(random_source);
        float dir_x = 0.0f;
        float dir_y = 0.0f;
        float speed = INITIAL_SPEED;
        float water = INITIAL_WATER_VOLUME;
        float sediment = 0.0f;

        std::cout << "Droplet @ " << pos_x << ", " << pos_y << std::endl;

        for (int lifetime = 0; lifetime < MAX_DROPLET_LIFETIME; lifetime++) {
            int node_x = (int)pos_x;
            int node_y = (int)pos_y;
            int droplet_index = node_y * width + node_x;

            float cell_offset_x = pos_x - node_x;
            float cell_offset_y = pos_y - node_y;

            // Calculate droplets height and direction
            HeightAndGradient height_and_gradient = CalculateHeightAndGradient(data, pos_x, pos_y);

            dir_x = (dir_x * INERTIA - height_and_gradient.gradient_x * (1 - INERTIA));
            dir_y = (dir_y * INERTIA - height_and_gradient.gradient_y * (1 - INERTIA));

            float len = std::sqrt(dir_x * dir_x + dir_y * dir_y);

            if (len != 0.0f) {
                dir_x /= len;
                dir_y /= len;
            }

            pos_x += dir_x;
            pos_y += dir_y;
            if ((dir_x == 0 && dir_y == 0) || pos_x < 0 || pos_x >= width - 1 || pos_x < 0 || pos_y >= height - 1) {
                break;
            }

            float new_height = CalculateHeightAndGradient(data, pos_x, pos_y).height;
            float delta_height = new_height = height_and_gradient.height;

            float sediment_capacity = std::max(
                -delta_height * speed * water *SEDIMENT_CAPACITY_FACTOR,
                MIN_SEDIMENT_CAPACITY
            );

            if (sediment > sediment_capacity || delta_height > 0.0f) {
                float amount_to_deposit = (delta_height > 0) ? std::min(delta_height, sediment) : (sediment - sediment_capacity) * DEPOSIT_SPEED;
                sediment -= amount_to_deposit;

                // TODO: This is most likely incorrect due to coordinate differences
                data[node_x][node_y].z += amount_to_deposit * (1 - cell_offset_x) * (1 - cell_offset_y);
                data[node_x + 1][node_y].z += amount_to_deposit * cell_offset_x * (1 - cell_offset_y);
                data[node_x][node_y + 1].z += amount_to_deposit * (1 - cell_offset_x) * cell_offset_y;
                data[node_x + 1][node_y + 1].z += amount_to_deposit * cell_offset_x * cell_offset_y;

                std::cout << "Depositing: " << amount_to_deposit << std::endl;
                std::cout << "Carrying: " << sediment << std::endl;
            } else {
                float amount_to_erode = std::min((sediment_capacity - sediment) * ERODE_SPEED, - delta_height);

                // TODO: What is erosion_brush_indices?
                //for (int brush_point_index = 0; brush_point_index < erosion_brush_indices[droplet_index].Length; brush_point_index++) {

                //}
            }

            speed = std::sqrt(speed * speed + delta_height * GRAVITY);
            water *= (1 - EVAPORATE_SPEED);

            std::cout << "New Position: " << pos_x << ", " << pos_y << std::endl;
        }
    }
}


int main() {
    std::cout << "Random" << std::endl;

    Image image(512, 512);
    Image image2(512, 512);
    V3 data[512][512];

    const int seed = 1337;
    const int octaves = 1;

    FastNoise myNoise;
    myNoise.SetNoiseType(FastNoise::SimplexFractal);
    myNoise.SetInterp(FastNoise::Quintic);
    myNoise.SetFrequency(0.005);
    myNoise.SetSeed(seed);
    myNoise.SetFractalGain(0.5f);
    myNoise.SetFractalLacunarity(2.0f);
    myNoise.SetFractalOctaves(octaves);

    FastNoise smallNoise;
    smallNoise.SetNoiseType(FastNoise::SimplexFractal);
    smallNoise.SetInterp(FastNoise::Quintic);
    smallNoise.SetFrequency(0.02);
    smallNoise.SetSeed(seed);
    smallNoise.SetFractalGain(0.5f);
    smallNoise.SetFractalLacunarity(2.0f);
    smallNoise.SetFractalOctaves(4);

    for (int y = 0; y < image.height(); ++y)
    {
        for (int x = 0; x < image.width(); ++x)
        {
            float sample = myNoise.GetNoise(x, y);
            float b = smallNoise.GetNoise(x, y);
            float scaled = (sample + 1.0f) / 2;
            float b_scaled = (b + 1.0f) / 2;
            float total = (scaled * 0.75) + (b_scaled * 0.25);
            const RGB color(total);
            const RGB colorb(b_scaled);

            image.set(x, y, color);
            image2.set(x, y, colorb);
            data[x][y] = V3{(float)x, (float)y, total};
        }
    }
    erode(data, 1);

    std::stringstream ss;
    ss << "noise" << ".bmp";
    if (image.saveBMP(ss.str()))
    {
        std::cout << "...saved \"" << ss.str() << "\"\n";
    }
    else
    {
        std::cout << "...failed\n";
    }
    std::stringstream ssb;
    ssb << "small" << ".bmp";
    image2.saveBMP(ssb.str());
    return 0;
}
