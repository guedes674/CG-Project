#include "shader.h"
#include <fstream>
#include <iostream>
#include <vector>

shader::shader() : program(0), vertex_shader(0), fragment_shader(0) {}

shader::~shader() {
    if (vertex_shader) glDeleteShader(vertex_shader);
    if (fragment_shader) glDeleteShader(fragment_shader);
    if (program) glDeleteProgram(program);
}

std::string shader::read_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open shader file: " << filename << std::endl;
        return "";
    }
    
    std::string content;
    std::string line;
    
    while (std::getline(file, line)) {
        content += line + "\n";
    }
    
    return content;
}

GLuint shader::compile_shader(GLenum type, const std::string& source) {
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    
    // Check compilation status
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    
    if (status != GL_TRUE) {
        GLint log_length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
        
        std::vector<char> log(log_length + 1);
        glGetShaderInfoLog(shader, log_length, nullptr, log.data());
        
        std::cerr << "shader compilation failed:\n" << log.data() << std::endl;
        
        glDeleteShader(shader);
        return 0;
    }
    
    return shader;
}

bool shader::load_from_file(const std::string& vertexFile, const std::string& fragmentFile) {
    std::string vertex_source = read_file(vertexFile);
    std::string fragment_source = read_file(fragmentFile);
    
    if (vertex_source.empty() || fragment_source.empty()) {
        return false;
    }
    
    vertex_shader = compile_shader(GL_VERTEX_SHADER, vertex_source);
    fragment_shader = compile_shader(GL_FRAGMENT_SHADER, fragment_source);
    
    if (!vertex_shader || !fragment_shader) {
        return false;
    }
    
    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    
    // Check linking status
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    
    if (status != GL_TRUE) {
        GLint log_length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
        
        std::vector<char> log(log_length + 1);
        glGetProgramInfoLog(program, log_length, nullptr, log.data());
        
        std::cerr << "Program linking failed:\n" << log.data() << std::endl;
        
        glDeleteProgram(program);
        program = 0;
        return false;
    }
    
    return true;
}

void shader::use() {
    if (program) {
        glUseProgram(program);
    }
}

void shader::disable() {
    glUseProgram(0);
}

void shader::set_uniform(const std::string& name, float value) {
    if (program) {
        GLint location = glGetUniformLocation(program, name.c_str());
        if (location != -1) {
            glUniform1f(location, value);
        }
    }
}

void shader::set_uniform(const std::string& name, int value) {
    if (program) {
        GLint location = glGetUniformLocation(program, name.c_str());
        if (location != -1) {
            glUniform1i(location, value);
        }
    }
}

void shader::set_uniform(const std::string& name, float x, float y, float z) {
    GLint location = glGetUniformLocation(program, name.c_str());
    if (location != -1) {
        glUniform3f(location, x, y, z);
    }
}