#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Terrain {
/*
A Flat mesh
*/
private:
    std::vector<glm::vec3> vertices;
    int width;
    int height;

public:
    Terrain(int width, int height);


};

