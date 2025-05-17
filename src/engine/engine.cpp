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
#include "post_processing.h"
#include "../aux/aux.h"
#include "../aux/point.h"
#include "../aux/curves.h"
#include "../xml/xml_parser.h"
using namespace std;

post_processing post_processor;

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
std::vector<vbo*> snapshot_models;
bool time_stop = false;
int last_time = 0;
int delta_time = 0;
float gl_last_matrix[16];
std::unordered_map<std::string, vbo*> model_dict;
std::unordered_map<std::string, GLuint> texture_dict;

int xml_id = 1;
std::unordered_map<int, Vector3> position_dict;
int current_group = 0;
std::vector<int> position_keys;
int current_target_index = -1; // -1 significa olhar para (0,0,0)

bool show_normals = false;
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

    post_processor.resize(w, h);
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

void apply_light(light current_light,GLenum light_enum){
    
    if (current_light.type == 0) {
        float light_array[4] = {current_light.x,current_light.y,current_light.z,1.0f};
        glLightfv(light_enum,GL_POSITION,light_array);
    }else {
        float light_array[4] = {current_light.x,current_light.y,current_light.z,0.0f};
        glLightfv(light_enum,GL_POSITION,light_array);
    }
}

void apply_light_spot(light_spot spot, GLenum light_enum){
    float light_pos[4] = {spot.point.x,spot.point.y,spot.point.z,1.0f};
    float light_dir[3] = {spot.direction.x,spot.direction.y,spot.direction.z};
    glLightfv(light_enum,GL_POSITION, light_pos);
    glLightfv(light_enum,GL_SPOT_DIRECTION, light_dir);
    glLightf(light_enum,GL_SPOT_CUTOFF,spot.cutoff);
}

void apply_lights(GLenum light_0){

    long unsigned int total_lights = 0;

    float white[4] = {1.0, 1.0, 1.0, 1.0};

    long unsigned int lights_size = parser.lights.lights.size();
    long unsigned int light_spots_size = parser.lights.light_spots.size();

    for (; total_lights < lights_size + light_spots_size;total_lights++){
        glLightfv(light_0 + total_lights, GL_AMBIENT, white);
        glLightfv(light_0 + total_lights, GL_DIFFUSE, white);
        glLightfv(light_0 + total_lights, GL_SPECULAR, white);
        if (total_lights < parser.lights.lights.size()) apply_light(parser.lights.lights[total_lights],light_0 + total_lights);
        else apply_light_spot(parser.lights.light_spots[total_lights - lights_size],light_0 + total_lights);
    }

}

void init_lights(GLenum light_0){
    long unsigned int total_lights = 0;

    glEnable(GL_LIGHTING);

    cout << "Total lights: " << parser.lights.lights.size() + parser.lights.light_spots.size() << endl;
    cout << "Total light spots: " << parser.lights.light_spots.size() << endl;

    for (; total_lights < parser.lights.lights.size() + parser.lights.light_spots.size();total_lights++) glEnable(light_0 + total_lights);

    if (total_lights == 0) glDisable(GL_LIGHTING);
}

/**
 * @brief Renders the scene with all models and visual elements
 */
void render_scene(void) {
    int time;
	float fps;
    static char text_fps[64];

    if (!time_stop){
        last_time = glutGet(GLUT_ELAPSED_TIME) - delta_time;
    }else {
        delta_time = glutGet(GLUT_ELAPSED_TIME) - last_time;
    }

    // Begin rendering to the framebuffer
    post_processor.begin_scene_render();

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

    apply_lights(GL_LIGHT0);

    float zero[3] = {0.0f,0.0f,0.0f};
    float red[3] = {1.0f,0.0f,0.0f};
    float green[3] = {0.0f,1.0f,0.0f};
    float blue[3] = {0.0f,0.0f,1.0f};
    float diffuse[3] = {0.8f, 0.8f, 0.8f};
    float ambient[3] = {0.2f, 0.2f, 0.2f};

    if (show_axes){
        glBegin(GL_LINES);
            glMaterialfv(GL_FRONT, GL_AMBIENT, zero);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, zero);
            glMaterialfv(GL_FRONT, GL_EMISSION, red);
            glColor3f(1.0f, 0.0f, 0.0f);   // x axis in red
            glVertex3f(-100.0f, 0.0f, 0.0f);
            glVertex3f(100.0f, 0.0f, 0.0f);
            glMaterialfv(GL_FRONT, GL_EMISSION, green);
            glColor3f(0.0f, 1.0f, 0.0f);   // y axis in green
            glVertex3f(0.0f, -100.0f, 0.0f);
            glVertex3f(0.0f, 100.0f, 0.0f);
            glMaterialfv(GL_FRONT, GL_EMISSION, blue);
            glColor3f(0.0f, 0.0f, 1.0f);   // z axis in blue
            glVertex3f(0.0f, 0.0f, -100.0f);
            glVertex3f(0.0f, 0.0f, 100.0f);
            glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
        glEnd();
    }
    glMaterialfv(GL_FRONT, GL_EMISSION, emissive_def);
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

    if (texture_dict.size()>0){

        // End rendering to the framebuffer
        post_processor.end_scene_render();

        // Apply post-processing effects and render to screen
        post_processor.render_post_processing();
    }
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
            show_normals = !show_normals;
            break;
        case 6:
            time_stop = !time_stop;
            break;
        case 7:
            snapshot = !snapshot;
            if((!time_stop && snapshot)||(time_stop && !snapshot)) time_stop = !time_stop;
            break;
        case 8:
            // Toggle between wireframe and filled polygons
            static bool wireframe_mode = false;
            wireframe_mode = !wireframe_mode;
            glPolygonMode(GL_FRONT_AND_BACK, wireframe_mode ? GL_LINE : GL_FILL);
            break;
        case 9:
            // Toggle CRT effect
            post_processor.toggle_crt_effect();
            break;
        case 10:
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
    glutAddMenuEntry("Show/Hide Normals",5);
    glutAddMenuEntry("Time Stop",6);  
    glutAddMenuEntry("Snapshot",7); 
    glutAddMenuEntry("Wireframe",8); 
    glutAddMenuEntry("Toggle CRT Effect",9);
    glutAttachMenu(GLUT_RIGHT_BUTTON);  
    glutMenuStatusFunc(menu_status);   

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glEnable(GL_RESCALE_NORMAL);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    init_lights(GL_LIGHT0);

    float amb[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb);

    for (const auto& g : parser.groups) {
        if (populate_dict(g, model_dict,texture_dict)) {
            cerr << "Failed to load models\n";
            return 1;
        }
    }

    // In main() after GLEW initialization and before glutMainLoop()
    if ((texture_dict.size()>0) && (!post_processor.init(window_width, window_height))) {
        cerr << "Failed to initialize post-processing effects\n";
        return 1;
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