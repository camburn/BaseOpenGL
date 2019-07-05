#include "terrain.hpp"

Terrain::Terrain(int width, int height): width(width), height(height) {
    int count = width * height;
    vertices.reserve(count);
    int z = 0;
    for (int x = 1; x < width; x++) {
        for (int y = 1; y < height; y++) {
            vertices.push_back(glm::vec3(x, y, z));
        }
    }
    std::cout << "Created terrain" << std::endl;
}
