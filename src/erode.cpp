#include "erode.hpp"

const float INITIAL_SPEED = 1.0f;
const float INITIAL_WATER_VOLUME = 1.0f;
const float MAX_DROPLET_LIFETIME = 50.0f;
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


HeightAndGradient CalculateHeightAndGradient(glm::vec3 data[], int size, float pos_x, float pos_y) {
    int coord_x = (int)pos_x;
    int coord_y = (int)pos_y;

    float x = pos_x - coord_x;
    float y = pos_y - coord_y;

    int node_index = coord_y * size + coord_x;
    float height_nw = data[node_index].y;
    float height_ne = data[node_index + 1].y;
    float height_sw = data[node_index + size].y;
    float height_se = data[node_index + size + 1].y;

    float gradient_x = (height_ne - height_nw) * (1 - y) + (height_se - height_sw) * y;
    float gradient_y = (height_sw - height_nw) * (1 - x) + (height_se - height_ne) * x;

    float cell_height = height_nw * (1 - x) * (1 - y) + height_ne * x * (1 - y) + height_sw * (1 - x) * y + height_se * x * y;

    return HeightAndGradient {cell_height, gradient_x, gradient_y};
}

int **brush_indices;
float **brush_weights;
int *brush_indices_sizes;

void calc_brush_indices(int mapsize, int radius) {
    brush_indices_sizes = new int[mapsize * mapsize];
    brush_indices = new int*[mapsize * mapsize];
    int brush_indices_length = mapsize * mapsize;

    brush_weights = new float*[mapsize * mapsize];

    int *x_offsets = new int[radius * radius * 4];
    int *y_offsets = new int[radius * radius * 4];
    float *weights = new float[radius * radius * 4];

    float weight_sum = 0;
    int add_index = 0;

    for (int i=0; i < brush_indices_length; i++) {
        int center_x = i % mapsize;
        int center_y = i / mapsize;

        if (center_y <= radius || center_y >= mapsize - radius || center_x <= radius + 1 || center_x >= mapsize - radius) {
            weight_sum = 0;
            add_index = 0;
            for (int y = -radius; y <= radius; y++) {
                for (int x = -radius; x <=radius; x++) {
                    float sqr_dst = x * x + y * y;
                    if (sqr_dst < radius * radius) {
                        int coord_x = center_x + x;
                        int coord_y = center_y + y;

                        if (coord_x >= 0 && coord_x < mapsize && coord_y >= 0 && coord_y < mapsize) {
                            float weight = 1 - std::sqrt(sqr_dst) / radius;
                            weight_sum += weight;
                            weights[add_index] = weight;
                            x_offsets[add_index] = x;
                            y_offsets[add_index] = y;
                            add_index++;
                        }
                    }
                }
            }
        }

        int num_entries = add_index;
        brush_indices_sizes[i] = num_entries;
        brush_indices[i] = new int[num_entries];
        brush_weights[i] = new float[num_entries];

        for (int j = 0; j < num_entries; j++) {
            brush_indices[i][j] = (y_offsets[j] + center_y) * mapsize + x_offsets[j] + center_x;
            brush_weights[i][j] = weights[j] / weight_sum;
        }
    }

    delete[] x_offsets;
    delete[] y_offsets;
    delete[] weights;
}

void erode(glm::vec3 data[], int size, const int iterations) {
    std::cout << "Eroding Terrain" << std::endl;
    std::default_random_engine random_source;
    std::uniform_real_distribution<float> random_x(0.0f, (float)size - 1);
    std::uniform_real_distribution<float> random_y(0.0f, (float)size - 1);

    // Create water droplet at random point on map
    for (int iteration = 0; iteration < iterations; iteration++) {
        float pos_x = random_x(random_source);
        float pos_y = random_y(random_source);

        float dir_x = 0.0f;
        float dir_y = 0.0f;
        float speed = INITIAL_SPEED;
        float water = INITIAL_WATER_VOLUME;
        float sediment = 0.0f;

        for (int lifetime = 0; lifetime < MAX_DROPLET_LIFETIME; lifetime++) {
            int node_x = (int)pos_x;
            int node_y = (int)pos_y;
            int droplet_index = node_y * size + node_x;

            float cell_offset_x = pos_x - node_x;
            float cell_offset_y = pos_y - node_y;

            // Calculate droplets height and direction
            HeightAndGradient height_and_gradient = CalculateHeightAndGradient(data, size, pos_x, pos_y);

            dir_x = (dir_x * INERTIA - height_and_gradient.gradient_x * (1 - INERTIA));
            dir_y = (dir_y * INERTIA - height_and_gradient.gradient_y * (1 - INERTIA));

            float len = std::sqrt(dir_x * dir_x + dir_y * dir_y);

            if (len != 0.0f) {
                dir_x /= len;
                dir_y /= len;
            }

            pos_x += dir_x;
            pos_y += dir_y;
            if ((dir_x == 0 && dir_y == 0) || pos_x < 0 || pos_x >= size - 1 || pos_y < 0 || pos_y >= size - 1) {
                break;
            }

            float new_height = CalculateHeightAndGradient(data, size, pos_x, pos_y).height;
            float delta_height = new_height - height_and_gradient.height;

            float sediment_capacity = std::max(-delta_height * speed * water * SEDIMENT_CAPACITY_FACTOR, MIN_SEDIMENT_CAPACITY);

            if (sediment > sediment_capacity || delta_height > 0.0f) {
                float amount_to_deposit = (delta_height > 0) ? std::min(delta_height, sediment) : (sediment - sediment_capacity) * DEPOSIT_SPEED;
                sediment -= amount_to_deposit;

                data[droplet_index].y += amount_to_deposit * (1 - cell_offset_x) * (1 - cell_offset_y);
                data[droplet_index + 1].y += amount_to_deposit * cell_offset_x * (1 - cell_offset_y);
                data[droplet_index + size].y += amount_to_deposit * (1 - cell_offset_x) * cell_offset_y;
                data[droplet_index + size + 1].y += amount_to_deposit * cell_offset_x * cell_offset_y;
            } else {
                float amount_to_erode = std::min((sediment_capacity - sediment) * ERODE_SPEED, -delta_height);

                for (int brush_point_index = 0; brush_point_index < brush_indices_sizes[droplet_index]; brush_point_index++) {
                    int node_index = brush_indices[droplet_index][brush_point_index];
                    float weighedErodeAmount = amount_to_erode * brush_weights[droplet_index][brush_point_index];
                    float delta_sediment = (data[node_index].y < weighedErodeAmount) ? data[node_index].y : weighedErodeAmount;

                    data[node_index].y -= delta_sediment;
                    sediment += delta_sediment;
                }
            }
            if ((speed * speed + delta_height * GRAVITY) < 0.0f)
                speed = 0.0f;
            else
                speed = std::sqrt(speed * speed + delta_height * GRAVITY);
            water *= (1 - EVAPORATE_SPEED);
        }
    }
}
