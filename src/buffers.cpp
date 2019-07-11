#include "buffers.hpp"

void CheckMessage(GLuint program_id) {
    int InfoLogLength;
    glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        char* ProgramErrorMessage = (char *)malloc(InfoLogLength);
        glGetProgramInfoLog(program_id, InfoLogLength, NULL, ProgramErrorMessage);
        fprintf(stderr, "Program Link Failure:\n %s\n", ProgramErrorMessage);
        free(ProgramErrorMessage);
    }
}


GLuint BufferMeshDataV(std::vector<glm::vec3> vertices) {
    // Create our VAO
    GLuint VertexArrayID;
    
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    // Buffer data to VBO
    GLuint vertexbuffer;
    
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(0);
    return VertexArrayID;
}

GLuint vertexbuffer;

GLuint BufferMeshDataV(glm::vec3 *vertices, const int size) {
    // Create our VAO
    GLuint VertexArrayID;

    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    // Buffer data to VBO
    //GLuint vertexbuffer;

    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, size * sizeof(glm::vec3), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(0);
    return VertexArrayID;
}

void UpdateMeshDataV(glm::vec3 *vertices, const int size) {
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, size * sizeof(glm::vec3), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

#ifdef SOIL_LIBRARY
GLuint BufferTextureDataFromFile(string file, string directory) {
    string filename = directory + file;
    replace(filename.begin(), filename.end(), '\\', '/');
    printf(":INFO::IMAGES:: Loading texture: %s\n:", filename.c_str());
    // Load Textures
    int tex_w, tex_h;
    unsigned char* image = SOIL_load_image(filename.c_str(), &tex_w, &tex_h, 0, SOIL_LOAD_RGB);

    if (!image) {
        printf("ERROR::IMAGES:: Failed to load texture %s\n", filename.c_str());
    }

    GLuint texture_id = BufferTextureDataRGB(image);
    SOIL_free_image_data(image);

    return texture_id;
}
#endif

GLuint BufferTextureDataRGB(unsigned char* data, const int tex_width, const int tex_height) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tex_width, tex_height, 0, GL_RGBA,
        GL_UNSIGNED_BYTE, data);

    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    return textureID;
}

GLuint LoadShader(std::string file_path, GLuint ShaderID) {   
    GLint Result = GL_FALSE;
    int InfoLogLength;
    std::string vertex_src_str = load_file(file_path);
    const char* vertex_src = vertex_src_str.c_str();
    // Compile the Shader
    std::cout << "Compiling Shader:" << file_path << std::endl;
    glShaderSource(ShaderID, 1, &vertex_src, NULL);
    glCompileShader(ShaderID);
    // Check result
    glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(ShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        char* ShaderErrorMessage = (char *)malloc(InfoLogLength);
        glGetShaderInfoLog(ShaderID, InfoLogLength, NULL, ShaderErrorMessage);
        std::cerr << "Shader Source" << std::endl << vertex_src << std::endl;
        std::cerr << "Shader Compile Failed:" << ShaderErrorMessage << std::endl;
        free(ShaderErrorMessage);
        throw std::runtime_error("Failed to compile shader");
    }
    return ShaderID;
}

GLuint BuildGlProgram(std::string vertex_file_path, std::string fragment_file_path) {
    GLuint vertexShaderID = LoadShader(vertex_file_path, glCreateShader(GL_VERTEX_SHADER));
    GLuint fragmentShaderID = LoadShader(fragment_file_path, glCreateShader(GL_FRAGMENT_SHADER));
    GLuint programID = glCreateProgram();
    GLint Result = GL_FALSE;
    GLint valid=1;

    std::cout << "Linking shaders to program" << std::endl;
    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);
    glLinkProgram(programID);

    glGetProgramiv(programID, GL_LINK_STATUS, &Result);
    if (!Result) {
        CheckMessage(programID);
        throw std::runtime_error("Failed to link shaders");
    }

    glValidateProgram(programID);
    glGetProgramiv(programID, GL_VALIDATE_STATUS, &valid);
    if (!valid) {
        CheckMessage(programID);
        throw std::runtime_error("Failed to validate program");
    }

    glUseProgram(0);
    glDetachShader(programID, vertexShaderID);
    glDetachShader(programID, fragmentShaderID);
    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);
    return programID;
}
