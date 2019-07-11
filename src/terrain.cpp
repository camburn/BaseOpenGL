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


inline void push_back(glm::vec3 vertices[], int &counter, glm::vec3 v) {
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
    delete image_data;
}

void Terrain::erode_mesh(const int iterations) {
    erosian_iterations += iterations;
    erode(data, size, iterations);
    buffer_data();
}

void Terrain::generate_mesh() {
    int size = 512;

    image_data = new RGB[size*size];

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
            unsigned char red = (unsigned char)(scaled_small * 255);
            unsigned char green = (unsigned char)(scaled_small * 255);
            unsigned char blue = (unsigned char)(scaled_small * 255);

            image_data[y * size + x] = RGB(red, green, blue);
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
            if (x != size - 1 && y != size - 1) {

                int index = y * size + x;

                // Calculate Triangle Normal
                glm::vec3 normal = glm::triangleNormal(
                    data[index + size] * scale,
                    data[index + 1] * scale,
                    data[index] * scale
                );

                // Construct the triangles
                push_back(verts, c, data[index + size] * scale);
                push_back(verts, c, glm::vec3(0.0f, 0.0f, 1.0f));// Barymetric
                push_back(verts, c, normal);

                push_back(verts, c, data[index + 1] * scale);
                push_back(verts, c, glm::vec3(0.0f, 1.0f, 0.0f));// Barymetric
                push_back(verts, c, normal);

                push_back(verts, c, data[index] * scale);
                push_back(verts, c, glm::vec3(1.0f, 0.0f, 0.0f));// Barymetric
                push_back(verts, c, normal);

                normal = glm::triangleNormal(
                    data[index + 1] * scale,
                    data[index + size] * scale,
                    data[index + size + 1] * scale
                );

                push_back(verts, c, data[index + 1] * scale);
                push_back(verts, c, glm::vec3(0.0f, 0.0f, 1.0f));// Barymetric
                push_back(verts, c, normal);

                push_back(verts, c, data[index + size] * scale);
                push_back(verts, c, glm::vec3(0.0f, 1.0f, 0.0f));// Barymetric
                push_back(verts, c, normal);

                push_back(verts, c, data[index + size + 1] * scale);
                push_back(verts, c, glm::vec3(1.0f, 0.0f, 0.0f));// Barymetric
                push_back(verts, c, normal);
            }
        }
    }
    std::cout << "Created Triangles" << std::endl;

    if (vao != 0) {
        UpdateMeshDataV(verts, array_size);
    }
    else {
        vao = BufferMeshDataV(verts, array_size);
    }
    vertices_size = array_size / 3;
    delete[] verts;

    image_id = BufferTextureDataRGB((unsigned char *)image_data, size, size);
}

void Terrain::draw() {
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertices_size);
    glBindVertexArray(0);
}

void Terrain::interface() {
    //TODO: move to class members
    static bool p_open = true;
    static int clicked = 0;
    static float f0 = 1000.0f;
    if (!ImGui::Begin("Terrain Controls", &p_open, 0)) {
        ImGui::End();
        return;
    }
    ImGui::Text("Terrain Controls description");
    if (ImGui::TreeNode("Erosion")) {
        ImGui::Text("Total Iterations ", std::to_string(erosian_iterations).c_str());

        if (ImGui::Button("Erode"))
            clicked++;
        ImGui::SameLine();
        ImGui::InputFloat("Iterations", &f0, 1.0f, 100.0f, "%.3f");

        if (clicked & 1)
        {
            ImGui::Text("Eroding Mesh!");
            if (f0 > 0.0f)
                erode_mesh(f0);
            clicked = 0;
        }
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Generation")) {
        ImGui::Text("Eroding Mesh!");
        ImGui::TreePop();
        
        if (image_id != 0) {
            ImTextureID id = &image_id;
            ImGui::Image(id, ImVec2(256, 256));

        }
        // Displpay perlin texture
        //
        // ImTextureID my_tex_id = io.Fonts->TexID;
        // float my_tex_w = (float)io.Fonts->TexWidth;
        // float my_tex_h = (float)io.Fonts->TexHeight;
        // ImGui::Text("%.0fx%.0f", my_tex_w, my_tex_h);
        // ImVec2 pos = ImGui::GetCursorScreenPos();
        // ImGui::Image(my_tex_id, ImVec2(my_tex_w, my_tex_h), ImVec2(0,0), 
        //    ImVec2(1,1), ImColor(255,255,255,255), ImColor(255,255,255,128));
    }
    ImGui::End();
}
