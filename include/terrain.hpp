#ifndef TERRAIN_INCLUDED
#define TERRAIN_INCLUDED

#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/normal.hpp>

#ifdef GL_CORE_PROFILE
#include <GL/gl3w.h>
#else
#include <GL/glew.h>
#endif

#include "buffers.hpp"
#include "erode.hpp"
#include "FastNoise.hpp"



class Terrain {
/*
A Flat mesh
*/
private:
    glm::vec3 *data;
    int vertices_size;
    int width;
    int height;
    int size;
    GLuint vao = -1;

public:
    int erosian_iterations = 0;
    Terrain(int width, int height);
    Terrain(glm::vec3 data[], int size);
    Terrain(int size);
    ~Terrain();

    void buffer_data();
    void Terrain::generate_mesh();
    void Terrain::erode_mesh(const int iterations);

    void draw();

};

#endif