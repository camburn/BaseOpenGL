#include "terrain.hpp"
#include "erode.hpp"

static std::map<std::string, FastNoise::NoiseType> NOISE_MAP = {
    {"SimplexFractal", FastNoise::SimplexFractal },
    {"Simplex", FastNoise::Simplex },
    {"Value", FastNoise::Value },
    {"ValueFractal", FastNoise::ValueFractal },
    {"Perlin", FastNoise::Perlin },
    {"PerlinFractal", FastNoise::PerlinFractal },
    {"Cellular", FastNoise::Cellular },
    {"WhiteNoise", FastNoise::WhiteNoise },
    {"Cubic", FastNoise::Cubic },
    {"CubicFractal", FastNoise::CubicFractal }
};

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


FastNoise Terrain::create_noise(
    float frequency, int fractal_octaves, int seed,
    float fractal_gain, float fractal_lacunarity,
    FastNoise::NoiseType noise_type
){
    
    FastNoise noise;
    noise.SetSeed(seed);
    noise.SetNoiseType(noise_type);
    noise.SetInterp(FastNoise::Quintic);
    noise.SetFrequency(frequency);
    noise.SetFractalGain(fractal_gain);
    noise.SetFractalLacunarity(fractal_lacunarity);
    noise.SetFractalOctaves(fractal_octaves);
    return noise;
}

void Terrain::preview_image(FastNoise noise) {
    if (preview_image_data != nullptr) {
        glDeleteTextures(1, &preview_image_data->texture_id);
        delete preview_image_data;
    }
    Color *image_data = new Color[size*size];
    for (int y = 0; y < size; ++y)
    {
        for (int x = 0; x < size; ++x)
        {
            float sample = noise.GetNoise(x, y);
            float scaled = (sample + 1.0f) / 2;
            unsigned char red = (unsigned char)(scaled * 255);
            unsigned char green = (unsigned char)(scaled * 255);
            unsigned char blue = (unsigned char)(scaled * 255);

            image_data[y * size + x] = Color(red, green, blue);
        }
    }
    preview_image_data = new Texture{"Combined", size,
        BufferTextureDataRGB((unsigned char *)image_data, size, size)
    };

    delete []image_data;
}

void Terrain::add_image(FastNoise noise) {

}

void Terrain::generate_mesh() {
    int size = 512;

    image_data = new Color[size*size];

    calc_brush_indices(size, 3);

    std::cout << "Generating mesh data" << std::endl;

    data = new glm::vec3[size * size];
    Color *image_data_combined = new Color[size*size];

    for (int y = 0; y < size; ++y)
    {
        for (int x = 0; x < size; ++x)
        {
            float height = 0.0f;
            for (auto noise: noises) {
                float sample = noise.GetNoise(x, y);
                float scaled = (sample + 1.0f) / 2;
                float weighted = scaled * noise.weight;

                noise.data[y * size + x] = Color((unsigned char)(scaled * 255));
                image_data_combined[y * size + x] = Color((unsigned char)(weighted * 255));
                height += weighted;
            }

            data[y * size + x] = glm::vec3{ (float)x, height, (float)y };
        }
    }

    for (auto &noise : noises) {
        //TODO: Can add the same image!
        if (!noise.added_image) {
            noise.added_image = true;
            images.emplace_back("Small features", size,
                BufferTextureDataRGB((unsigned char *)noise.data, size, size)
            );
            images.back().noise = &noise;
        }
    }

    delete []image_data_combined;
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
}

void Terrain::draw() {
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertices_size);
    glBindVertexArray(0);
}

void Terrain::interface() {
    //TODO: move to class members
    static bool p_texture_gen_open = true;
    { // Controls
        static bool p_open = true;
        static int clicked = 0;
        static float f0 = 1000.0f;
        if (!ImGui::Begin("Terrain Controls", &p_open, 0)) {
            ImGui::End();
            return;
        }
        ImGui::Text("Terrain Controls description");
        if (ImGui::TreeNode("Erosion")) {
            ImGui::LabelText("Total Iterations", "%s", std::to_string(erosian_iterations).c_str());
            ImGui::LabelText("Total noises", "%s", std::to_string(noises.size()).c_str());

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
            ImGui::Text("Texture base hightmaps");
            
            int counter = 0;
            int image_to_delete = -1;
            for (auto i: images) {
                if (ImGui::TreeNode(i.name.c_str())) {
                    if (ImGui::Button("Delete")) {
                        image_to_delete = counter;
                        delete[] i.noise->data;
                        //TODO: Delete noise

                    }
                    ImGui::LabelText("Weight", "%s", std::to_string(i.noise->weight).c_str());
                    ImGui::Image((void*)(intptr_t)i.texture_id, ImVec2(128, 128));
                    ImGui::TreePop();
                }
                counter++;
            }
            if (image_to_delete > 0) {
                images.erase(images.begin() + image_to_delete);
            }
            
            ImGui::TreePop();

        }
        ImGui::End();
    }
    { // Texture Generator
        static int add_clicked = 0;
        static int preview_clicked = 0;
        static float weight = 1.0f;
        static float frequency = 0.02f;
        static int fractal_octaves = 8;
        static int seed = 1337;
        static float fractal_gain = 0.3f;
        static float fractal_lacunarity = 2.0f;
        if (!ImGui::Begin("Texture Generator", &p_texture_gen_open, 0)) {
            ImGui::End();
            return;
        }

        ImGui::InputFloat("Weight", &weight, 0.1f, 0.25f, "%.5f");
        ImGui::InputFloat("Frequency", &frequency, 0.001f, 0.01f, "%.5f");
        ImGui::InputInt("Fractal Octaves", &fractal_octaves, 1, 2);
        ImGui::InputInt("Seed", &seed);
        ImGui::InputFloat("Fractal Gain", &fractal_gain, 0.001f, 0.01f, "%.5f");
        ImGui::InputFloat("Fractal Lacunarity", &fractal_lacunarity, 0.5f, 1.0f, "%.2f");

        const char* type_strings[] = {
            "SimplexFractal",
            "Simplex",
            "Value",
            "ValueFractal",
            "Perlin",
            "PerlinFractal",
            "Cellular",
            "WhiteNoise",
            "Cubic",
            "CubicFractal",
        };
        static const char* item_current = type_strings[0];

        if (ImGui::BeginCombo("Noise Type", item_current)) {
            for (int n = 0; n < IM_ARRAYSIZE(type_strings); n++)
            {
                bool is_selected = (item_current == type_strings[n]);
                if (ImGui::Selectable(type_strings[n], is_selected))
                    item_current = type_strings[n];
                if (is_selected)
                    ImGui::SetItemDefaultFocus();   // Set the initial focus when opening the combo (scrolling + for keyboard navigation support in the upcoming navigation branch)
            }
            ImGui::EndCombo();
        }

        if (ImGui::Button("Preview"))
            preview_clicked++;
        ImGui::SameLine();
        if (ImGui::Button("Add"))
            add_clicked++;

        if (preview_clicked & 1) {
            
            FastNoise noise = create_noise(
                frequency, fractal_octaves, seed, fractal_gain, fractal_lacunarity,
                NOISE_MAP[std::string(item_current)]
            );
            preview_image(noise);
            preview_clicked = 0;
        }
        if (add_clicked & 1) {
            FastNoise noise = create_noise(
                frequency, fractal_octaves, seed, fractal_gain, fractal_lacunarity,
                NOISE_MAP[std::string(item_current)]
            );
            noises.emplace_back(
                size, noise, weight
            );
            add_clicked = 0;
            generate_mesh();
            buffer_data();
        }

        if (preview_image_data != nullptr) {
            ImGui::Image((void*)(intptr_t)preview_image_data->texture_id, ImVec2(256, 256));
        }

        ImGui::End();
    }
}


