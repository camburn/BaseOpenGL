#include "terrain.hpp"

Terrain::Terrain(int width, int height): width(width), height(height) {

    int count = width * height;
    vertices.reserve(count * 2);
    int y = 0;
    
    for (int z = 0; z < height; z++) {
        for (int x = 0; x < width; x++) {
    
            // Triangle 1
            
            vertices.push_back(glm::vec3(x, y, z));
            vertices.push_back(glm::vec3(1.0f, 0.0f, 0.0f));// Barymetric
            vertices.push_back(glm::vec3(x + 1, y, z));
            vertices.push_back(glm::vec3(0.0f, 1.0f, 0.0f));// Barymetric
            vertices.push_back(glm::vec3(x, y, z + 1));
            vertices.push_back(glm::vec3(0.0f, 0.0f, 1.0f));// Barymetric
            
            // Triangle 2
            vertices.push_back(glm::vec3(x + 1, y, z + 1));
            vertices.push_back(glm::vec3(1.0f, 0.0f, 0.0f));// Barymetric
            vertices.push_back(glm::vec3(x, y, z + 1));
            vertices.push_back(glm::vec3(0.0f, 1.0f, 0.0f));// Barymetric
            vertices.push_back(glm::vec3(x + 1, y, z));
            vertices.push_back(glm::vec3(0.0f, 0.0f, 1.0f));// Barymetric
            
        }
    }

    std::cout << "Created terrain" << std::endl;

    vao = BufferMeshDataV(vertices);
}

void Terrain::draw() {
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, (vertices.size()/2) * 3);
    glBindVertexArray(0);
} 
