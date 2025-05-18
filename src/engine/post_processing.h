#ifndef POST_PROCESSING_H
#define POST_PROCESSING_H

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/gl.h>
#endif
#include "shaders/shader.h"
#include "../xml/xml_parser.h"

extern xml_parser parser;

class post_processing {
    public:
        post_processing();
        ~post_processing();
        
        bool init(int windowWidth, int windowHeight);
        void begin_scene_render();
        void end_scene_render();
        void render_post_processing();
        void resize(int width, int height);
        
        bool is_crt_enabled() const { return crt_enabled; }
        void toggle_crt_effect() { crt_enabled = !crt_enabled; }

        void toggle_outline() { outline_enabled = !outline_enabled; }
        bool is_outline_enabled() const { return outline_enabled; }
        
    private:
        GLuint fbo;             // Framebuffer object
        GLuint render_texture;   // Texture to render the scene to
        GLuint depth_render_buffer; // Depth buffer
        
        int width, height;      // Window dimensions
        bool crt_enabled;        // Whether CRT effect is enabled
        
        shader crt_shader;       // Shader for CRT effect
        
        shader outline_shader;
        bool outline_enabled;
        GLuint depth_texture;

        void create_fullscreen_quad();
};

#endif // POST_PROCESSING_H