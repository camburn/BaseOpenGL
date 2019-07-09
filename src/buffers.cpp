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
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(0);
    return VertexArrayID;
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