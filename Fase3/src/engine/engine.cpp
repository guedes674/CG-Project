#include <stdlib.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/gl.h>
#endif

#define _USE_MATH_DEFINES

#include <math.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <climits>
#include <unordered_map>
#include <algorithm>

#include "camera.h"
#include "model_handling.h"
#include "input_handling.h"
#include "../aux/aux.h"
#include "../aux/point.h"
#include "../aux/curves.h"
#include "../xml/xml_parser.h"
using namespace std;

xml_parser parser;

// --- CAMERA VARIABLES ---
Camera camera;

int window_width  = 800;
int window_height = 600;
int last_mouse_x  = 0;
int last_mouse_y  = 0;

// --- MODEL VARIABLES ---
int global_id = 0;

int current_models      = 0;
bool snapshot = false;
int last_time = 0;

std::unordered_map<std::string, vbo*> model_dict;

int xml_id = 1;
std::unordered_map<int, Vector3> position_dict;
int current_group = 0;
std::vector<int> position_keys;
int current_target_index = -1; // -1 significa olhar para (0,0,0)

bool show_bounding_box = false;
bool show_catmull_curves = true;
bool show_axes = true;
// --- INPUT HANDLING VARIABLES ---
bool mouse_dragging = false;

bool keys[256] = {false}; // Track regular keys
bool special_keys[256] = {false}; // Track special keys

float scale = 1.0f;

int last_frame_time = 0;

// --- TEXT VARIABLES ---
float emissive_def[4] = {0.0f,0.0f,0.0f,1.0f};
float emissive_full[4] = {1.0f,1.0f,1.0f,1.0f};
int timebase = 0, frame = 0;

/**
 * @brief Initializes camera parameters from XML data
 */
void init_camera() {
    camera.init_from_xml(parser);
}

/**
 * @brief Handles window resize events and updates the projection matrix
 * 
 * @param w The new window width
 * @param h The new window height
 */
void change_size(int w, int h) {
    if(h == 0) h = 1;
    float ratio = w * 1.0 / h;
    window_width = w;
    window_height = h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, w, h);
    gluPerspective(parser.cam.fov, ratio, parser.cam.near, parser.cam.far);
    glMatrixMode(GL_MODELVIEW);
}

/**
 * @brief Renders text on the screen at a specified position
 * 
 * @param text The text to render
 * @param posx The x-coordinate of the text position
 * @param posy The y-coordinate of the text position
 */
void render_text(const std::string text,double posx, double posy) {
    // Guardar a projeção anterior
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    
    // Projecção ortogonal…
    gluOrtho2D(0, window_width, window_height, 0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    void* font = GLUT_BITMAP_HELVETICA_18;
    glDisable(GL_DEPTH_TEST);
    glColor3f(1.0f, 1.0f, 1.0f);
    glMaterialfv(GL_FRONT, GL_EMISSION, emissive_full);
    glRasterPos2d(posx, posy);
    for (char c : text) glutBitmapCharacter(font, c);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glEnable(GL_DEPTH_TEST);
    glMaterialfv(GL_FRONT, GL_EMISSION, emissive_def);
}

/**
 * @brief Renders the scene with all models and visual elements
 */
void render_scene(void) {
    int time;
	float fps;
    static char text_fps[64];

    if (!snapshot)last_time = glutGet(GLUT_ELAPSED_TIME);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Set up view matrix based on camera mode
    if(camera.mode == Camera::ORBIT) {
        // Calculate orbit position
        float eye_x = camera.orbit_look_at.x + camera.orbit_radius * cos(camera.orbit_polar) * sin(camera.orbit_azimuth);
        float eye_y = camera.orbit_look_at.y + camera.orbit_radius * sin(camera.orbit_polar);
        float eye_z = camera.orbit_look_at.z + camera.orbit_radius * cos(camera.orbit_polar) * cos(camera.orbit_azimuth);
        gluLookAt(eye_x, eye_y, eye_z,
                camera.orbit_look_at.x, camera.orbit_look_at.y, camera.orbit_look_at.z,
                parser.cam.ux, parser.cam.uy, parser.cam.uz);
    } else { // FPS mode
        Vector3 look_at = camera.fps_position + camera.fps_front;
        gluLookAt(camera.fps_position.x, camera.fps_position.y, camera.fps_position.z,
                look_at.x, look_at.y, look_at.z,
                camera.fps_up.x, camera.fps_up.y, camera.fps_up.z);
    }

    // Draw the axes
    if (show_axes){
        glBegin(GL_LINES);
            glColor3f(1.0f, 0.0f, 0.0f);   // x axis in red
            glVertex3f(-100.0f, 0.0f, 0.0f);
            glVertex3f(100.0f, 0.0f, 0.0f);

            glColor3f(0.0f, 1.0f, 0.0f);   // y axis in green
            glVertex3f(0.0f, -100.0f, 0.0f);
            glVertex3f(0.0f, 100.0f, 0.0f);

            glColor3f(0.0f, 0.0f, 1.0f);   // z axis in blue
            glVertex3f(0.0f, 0.0f, -100.0f);
            glVertex3f(0.0f, 0.0f, 100.0f);
        glEnd();
    }

    glScalef(scale, scale, scale);
    for(const auto& group : parser.groups)
        recursive_draw(group);

    frame++;
    time = glutGet(GLUT_ELAPSED_TIME);
    if (time - timebase > 1000) {
        fps = frame * 1000.0f / (time - timebase);
        timebase = time;
        frame = 0;
        snprintf(text_fps, sizeof(text_fps), "FPS: %.3f", fps);
    }

    render_text(text_fps, window_width/40, window_height/35);

    static char text[120];

    if (camera.mode == Camera::FPS) {
        snprintf(text, sizeof(text), "FPS Yaw: %.3f", camera.fps_yaw);
        render_text(text, window_width/40, window_height/35 + 20);
        snprintf(text, sizeof(text),
            "Look x: %.3f y: %.3f z: %.3f",
            camera.fps_front.x, camera.fps_front.y, camera.fps_front.z);
        render_text(text, window_width/40, window_height/35 + 40);
        snprintf(text, sizeof(text),
            "Pos x: %.3f y: %.3f z: %.3f",
            camera.fps_position.x, camera.fps_position.y, camera.fps_position.z);
        render_text(text, window_width/40, window_height/35 + 60);
    } else {
        snprintf(text, sizeof(text), "Radius α: %.3f", camera.orbit_radius);
        render_text(text, window_width/40, window_height/35 + 20);
        snprintf(text, sizeof(text),
            "Look x: %.3f y: %.3f z: %.3f",
            camera.orbit_look_at.x, camera.orbit_look_at.y, camera.orbit_look_at.z);
        render_text(text, window_width/40, window_height/35 + 40);
        snprintf(text, sizeof(text),
            "Pos x: %.3f y: %.3f z: %.3f",
            camera.orbit_radius * cos(camera.orbit_polar) * sin(camera.orbit_azimuth),
            camera.orbit_radius * sin(camera.orbit_polar),
            camera.orbit_radius * cos(camera.orbit_polar) * cos(camera.orbit_azimuth));
        render_text(text, window_width/40, window_height/35 + 60);
    }

    snprintf(text, sizeof(text), "Numero de modelos: %d", current_models);
    render_text(text, window_width/40, window_height/35 + 80);
    current_models = 0;

    glutSwapBuffers();
}

/**
 * @brief Timer callback for continuous rendering and input processing
 * 
 * @param value Value passed to this function (unused)
 */
void timer_func(int value) {
    (void)value;
    int current_time = glutGet(GLUT_ELAPSED_TIME);
    last_frame_time = current_time;

    update_camera();
    glutPostRedisplay();
    glutTimerFunc(16, timer_func, 0); // Use fixed 60fps timing (16ms)
}

void menu(int value) {

    switch(value) {
        case 1:
            camera.toggle_mode();
            glutSetWindowTitle(
                camera.mode == Camera::ORBIT ? "CG@33 - Orbit Mode" : "CG@33 - FPS Mode"
            );
            break;
        case 2:
            show_bounding_box = !show_bounding_box;
            break;
        case 3:
            show_catmull_curves = !show_catmull_curves;
            break;
        case 4:
            show_axes = !show_axes;
            break;
        case 5:
            snapshot = !snapshot;
            break;
        case 9:
            exit(0);
            break;
        default:
            break;
    }
}

void menu_status(int status, int x, int y) {
    (void)x; (void)y;
    if (status == GLUT_MENU_IN_USE) {
        std::fill_n(keys,         256, false);
        std::fill_n(special_keys, 256, false);
    }
}

/**
 * @brief Main application entry point
 * 
 * @param argc Number of command-line arguments
 * @param argv Array of command-line argument strings
 * @return int 0 on successful execution, non-zero on error
 */
int main(int argc, char** argv) {
    if(argc != 2) {
        cerr << "Usage: " << argv[0] << " <scene.xml>\n";
        return 1;
    }

    parser = read_xml_file(argv[1]);
    init_camera();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(800, 800);
    glutCreateWindow("CG@33 - Orbit Mode");

    #ifndef __APPLE__
    // Only initialize GLEW on non-Apple platforms (Linux/Windows)
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        cerr << "Error initializing GLEW: " << glewGetErrorString(err) << endl;
        return 1;
    }
    #endif

    glutDisplayFunc(render_scene);
    glutIdleFunc(render_scene);
    glutReshapeFunc(change_size);

    glutKeyboardFunc(default_key_func);
    glutKeyboardUpFunc(default_key_up_func);
    glutSpecialFunc(special_key_func);
    glutSpecialUpFunc(special_key_up_func);
    
    glutMotionFunc(process_mouse_motion);
    glutMouseFunc(process_mouse_buttons);
    glutTimerFunc(0, timer_func, 0);

    camera.update_cursor_mode();
    glutCreateMenu(menu);  
    glutAddMenuEntry("Change Cam Mode",1);
    glutAddMenuEntry("Show/Hide Bounding Boxes",2);
    glutAddMenuEntry("Show/Hide Catmull-Rom Curves",3);
    glutAddMenuEntry("Show/Hide Axes",4);
    glutAddMenuEntry("Snapshot",5);    
    glutAttachMenu(GLUT_RIGHT_BUTTON);  
    glutMenuStatusFunc(menu_status);   

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    for (const auto& g : parser.groups) {
        if (populate_dict(g, model_dict)) {
            cerr << "Failed to load models\n";
            return 1;
        }
    }

    glutMainLoop();
    
    // Cleanup
    for(auto& entry : model_dict) {
        glDeleteBuffers(1, &entry.second->vertices);
        glDeleteBuffers(1, &entry.second->indexes);
        delete entry.second;
    }

    return 0;
}