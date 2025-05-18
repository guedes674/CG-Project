#include "post_processing.h"
#include <iostream>

post_processing::post_processing()
    : fbo(0), render_texture(0), depth_render_buffer(0), width(0), height(0), crt_enabled(false) {}

post_processing::~post_processing() {
    if (render_texture) glDeleteTextures(1, &render_texture);
    if (depth_render_buffer) glDeleteRenderbuffers(1, &depth_render_buffer);
    if (fbo) glDeleteFramebuffers(1, &fbo);
}

bool post_processing::init(int window_width, int window_height) {
    width = window_width;
    height = window_height;
    
    // Load shaders
    if (!crt_shader.load_from_file("src/engine/shaders/crt/crt.vert", "src/engine/shaders/crt/crt.frag")) {
        std::cerr << "Failed to load CRT shader" << std::endl;
        return false;
    }
    
    if (!outline_shader.load_from_file("src/engine/shaders/outline/outline.vert", "src/engine/shaders/outline/outline.frag")) {
        std::cerr << "Failed to load outline shader" << std::endl;
        return false;
    }
    
    // Create framebuffer   
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    
    // Create color texture
    glGenTextures(1, &render_texture);
    glBindTexture(GL_TEXTURE_2D, render_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    // Create depth texture (instead of renderbuffer)
    glGenTextures(1, &depth_texture);
    glBindTexture(GL_TEXTURE_2D, depth_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    // Attach textures to framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, render_texture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_texture, 0);
    
    // Check FBO status
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer is not complete!" << std::endl;
        return false;
    }
    
    // Switch back to default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}

void post_processing::resize(int width, int height) {
    this->width = width;
    this->height = height;
    
    // Resize color texture
    glBindTexture(GL_TEXTURE_2D, render_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    
    // Resize depth texture
    glBindTexture(GL_TEXTURE_2D, depth_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
}

void post_processing::begin_scene_render() {
    // Bind the framebuffer to render the scene to a texture
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void post_processing::end_scene_render() {
    // Unbind the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void post_processing::render_post_processing() {

    // Disable depth testing for full-screen quad
    glDisable(GL_DEPTH_TEST);
    
    // Disable lighting for the post-processing quad
    glDisable(GL_LIGHTING);
    
    // Bind the render texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, render_texture);
    
    // If using outline effect, bind depth texture as well
    if (outline_enabled) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depth_texture);
    }
    
    // Select which shader to use
    if (crt_enabled) {
        crt_shader.use();
        crt_shader.set_uniform("screenTexture", 0);
        crt_shader.set_uniform("time", static_cast<float>(glutGet(GLUT_ELAPSED_TIME)) / 1000.0f);
    } else if (outline_enabled) {
        outline_shader.use();
        outline_shader.set_uniform("colorTexture", 0);
        outline_shader.set_uniform("depthTexture", 1);
        outline_shader.set_uniform("outlineWidth", 1.0f);  // Adjust as needed
        outline_shader.set_uniform("outlineColor", 1.0f, 1.0f, 1.0f);  // White
    }
    
    // Draw a full-screen quad
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, -1, 1);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex2f(-1, -1);
        glTexCoord2f(1, 0); glVertex2f(1, -1);
        glTexCoord2f(1, 1); glVertex2f(1, 1);
        glTexCoord2f(0, 1); glVertex2f(-1, 1);
    glEnd();
    
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    
    
    // Disable shaders
    if (crt_enabled) {
        crt_shader.disable();
    } else if (outline_enabled) {
        outline_shader.disable();
    }
    
    // Reset active texture
    glActiveTexture(GL_TEXTURE0);
    
    // Re-enable lighting if necessary
    if (parser.lights.lights.size() + parser.lights.light_spots.size() > 0) {
        glEnable(GL_LIGHTING);
    }
    
    // Re-enable depth testing
    glEnable(GL_DEPTH_TEST);

    
}