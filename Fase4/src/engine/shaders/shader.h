#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <GL/glew.h>

class shader {
    public:
        shader();
        ~shader();
        
        bool load_from_file(const std::string& vertexFile, const std::string& fragmentFile);
        void use();
        void disable();
        
        void set_uniform(const std::string& name, float value);
        void set_uniform(const std::string& name, int value);
        void set_uniform(const std::string& name, float x, float y, float z);
        
        GLuint get_program() { return program; }
        
    private:
        GLuint program;
        GLuint vertex_shader;
        GLuint fragment_shader;
        
        std::string read_file(const std::string& filename);
        GLuint compile_shader(GLenum type, const std::string& source);
};

#endif // SHADER_H