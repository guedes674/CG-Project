#ifndef INPUT_HANDLING_H
#define INPUT_HANDLING_H

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/gl.h>
#endif

#include "camera.h"
#include "model_handling.h"

extern Camera camera;
extern int window_width;
extern int window_height;
extern int last_mouse_x;
extern int last_mouse_y;
extern bool mouse_dragging;
extern bool keys[256];
extern bool special_keys[256];
extern int last_frame_time;
extern float scale;
extern int current_target_index;
extern std::unordered_map<std::string, vbo*> model_dict;
extern bool show_axes;

void process_mouse_motion(int x, int y);
void process_mouse_buttons(int button, int state, int x, int y);
void update_camera();
void default_key_func(unsigned char key, int x, int y);
void default_key_up_func(unsigned char key, int x, int y);
void special_key_func(int key, int x, int y);
void special_key_up_func(int key, int x, int y);
void change_size(int w, int h);
void timer_func(int value);

#endif