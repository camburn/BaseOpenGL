#include "terrain.hpp"
#include "erode.hpp"


glm::vec3 calc_normal(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
    glm::vec3 U = p2 - p1;
    glm::vec3 V = p3 - p1;

    glm::vec3 normal(1.0f);

    normal.x = (U.y * V.z) - (U.z * V.y);
    normal.y = (U.z * V.x) - (U.x * V.z);
    normal.z = (U.x * V.y) - (U.y * V.x);

    return normal;
}


inline void push_back(glm::vec3 vertices[], int &counter, glm::vec3 &v) {
    vertices[counter] = v;
    counter++;
}

Terrain::Terrain(glm::vec3 data[], int size): data(data), size(size) {
    buffer_data();
}

Terrain::Terrain(int size): size(size) {
    generate_mesh();
    buffer_data();
}

Terrain::~Terrain() {
    delete data;
}

void Terrain::erode_mesh(const int iterations) {
    erosian_iterations += iterations;
    erode(data, size, iterations);
    buffer_data();
}

void Terrain::generate_mesh() {

    int size = 512;

    calc_brush_indices(size, 3);

    const int seed = 1337;
    const int octaves = 1;

    std::cout << "Generating mesh data" << std::endl;
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

    data = new glm::vec3[size * size];

    for (int y = 0; y < size; ++y)
    {
        for (int x = 0; x < size; ++x)
        {
            float sample_big = big_noise.GetNoise(x, y);
            float sample_small = small_noise.GetNoise(x, y);
            float scaled_big = (sample_big + 1.0f) / 2;
            float scaled_small = (sample_small + 1.0f) / 2;
            float height = (scaled_big * 0.9) + (scaled_small * 0.1);
            data[y * size + x] = glm::vec3{ (float)x, height, (float)y };
        }
    }
}

void Terrain::buffer_data() {
    std::cout << "Generating Triangles" << std::endl;
    const int array_size = size * size * 18;
    glm::vec3 *verts = new glm::vec3[array_size];

    glm::vec3 scale(0.01f, 2.0f, 0.01f);

    int c = 0;
    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x) {

            // Remove me?
            if (x != size - 1 && y != size - 1) {

                int index = y * size + x;

                // Calculate Triangle Normal
                glm::vec3 normal = glm::triangleNormal(
                    data[index + size] * scale,
                    data[index + 1] * scale,
                    data[index] * scale
                );

                // Construct the triangles
                push_back(verts, c, data[index] * scale);
                push_back(verts, c, glm::vec3(1.0f, 0.0f, 0.0f));// Barymetric
                push_back(verts, c, normal);

                push_back(verts, c, data[index + 1] * scale);
                push_back(verts, c, glm::vec3(0.0f, 1.0f, 0.0f));// Barymetric
                push_back(verts, c, normal);

                push_back(verts, c, data[index + size] * scale);
                push_back(verts, c, glm::vec3(0.0f, 0.0f, 1.0f));// Barymetric
                push_back(verts, c, normal);

                normal = glm::triangleNormal(
                    data[index + 1] * scale,
                    data[index + size] * scale,
                    data[index + size + 1] * scale
                );

                push_back(verts, c, data[index + size + 1] * scale);
                push_back(verts, c, glm::vec3(1.0f, 0.0f, 0.0f));// Barymetric
                push_back(verts, c, normal);

                push_back(verts, c, data[index + size] * scale);
                push_back(verts, c, glm::vec3(0.0f, 1.0f, 0.0f));// Barymetric
                push_back(verts, c, normal);

                push_back(verts, c, data[index + 1] * scale);
                push_back(verts, c, glm::vec3(0.0f, 0.0f, 1.0f));// Barymetric
                push_back(verts, c, normal);
            }
        }
    }
    std::cout << "Created Triangles" << std::endl;

    if (vao != -1) {
        UpdateMeshDataV(verts, array_size);
    }
    else {
        vao = BufferMeshDataV(verts, array_size);
    }
    
    vertices_size = array_size / 3;
    delete verts;
}

void Terrain::draw() {
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertices_size);
    glBindVertexArray(0);
} 
