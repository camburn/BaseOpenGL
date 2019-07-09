#ifndef TERRAIN_INCLUDED
#define TERRAIN_INCLUDED

#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#ifdef GL_CORE_PROFILE
#include <GL/gl3w.h>
#else
#include <GL/glew.h>
#endif

#include "buffers.hpp"



class Terrain {
/*
A Flat mesh
*/
private:
    std::vector<glm::vec3> vertices;
    int width;
    int height;
    GLuint vao;

public:
    Terrain(int width, int height);
    void draw();

};

#endif