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
#include "imgui.h"


struct RGB{
    RGB():r(0), b(0), g(0), a(0){}
    RGB(unsigned char r, unsigned char b, unsigned char g):
        r(r), b(b), g(g), a(255){}
    RGB(unsigned char r, unsigned char b, unsigned char g, unsigned char a):
        r(r), b(b), g(g), a(a){}
    unsigned char r, b, g, a;
};

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
    
    GLuint vao = 0;
    RGB *image_data;

    std::vector<GLuint> images;

public:
    GLuint image_id = 0;
    int erosian_iterations = 0;
    Terrain(int width, int height);
    Terrain(glm::vec3 data[], int size);
    Terrain(int size);
    ~Terrain();

    void buffer_data();
    void generate_mesh();
    void erode_mesh(const int iterations);

    void interface();
    void draw();

};

#endif
