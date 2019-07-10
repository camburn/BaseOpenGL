#ifndef __ERODE_HPP
#define __ERODE_HPP
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <random>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


void calc_brush_indices(int mapsize, int radius);
void erode(glm::vec3 data[], int size, const int iterations);

#endif