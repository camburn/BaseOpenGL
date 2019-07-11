/*
This is for the management of buffers to OpenGL
It manages data into OpenGL buffers and prepares it for drawing.
*/
#ifndef BUFFERS_INCLUDED
#define BUFFERS_INCLUDED

#include <assert.h>
#include <algorithm>
#include <string>
#include <vector>
#include <map>
#include <iostream>

#include <stdio.h>
#include <glm/glm.hpp>

#ifdef GL_CORE_PROFILE
#include <GL/gl3w.h>
#else
#include <GL/glew.h>
#endif

#include "io.hpp"

class TextureBuffer{
public:
	GLuint framebuffer;
	GLuint texture_id;
};

GLuint BufferMeshDataV(std::vector<glm::vec3> vertices);
GLuint BufferMeshDataV(glm::vec3 vertices[], const int size);
GLuint BufferTextureDataRGB(unsigned char* data, const int tex_width, const int tex_height);
void UpdateMeshDataV(glm::vec3 *vertices, const int size);
GLuint LoadShader(std::string file_path, GLuint ShaderID);
GLuint BuildGlProgram(std::string vertex_file_path, std::string fragment_file_path);

#endif
