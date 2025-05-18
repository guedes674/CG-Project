#ifndef CAMERA_H
#define CAMERA_H

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/gl.h>
#endif

#define _USE_MATH_DEFINES
#include <cmath>
#include "../aux/point.h"
#include "../xml/xml_parser.h"

extern int window_width;
extern int window_height;
extern int last_mouse_x;
extern int last_mouse_y;

struct Camera{
    enum Mode { ORBIT, FPS };
    Mode mode = ORBIT;

    // Common parameters
    float sensitivity = 0.1f;
    float sensitivity_orbit = 0.005f;
    float move_speed = 0.20f;

    // Orbit mode parameters
    float orbit_azimuth = 0.0f;
    float orbit_polar = 0.0f;
    float orbit_radius = 5.0f;
    Vector3 orbit_look_at;

    // FPS mode parameters
    Vector3 fps_position;
    Vector3 fps_front;
    Vector3 fps_right;
    Vector3 fps_up;
    float fps_yaw = -90.0f;
    float fps_pitch = 0.0f;

    void init_from_xml(xml_parser& parser);
    void update_fps_vectors();
    void update_cursor_mode();
    void toggle_mode();
    void follow_target(const Vector3&  new_target_position);
    Vector3 get_orbit_camera_position() const;
};

#endif