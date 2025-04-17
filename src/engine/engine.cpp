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
#include "../generator/model.h"
#include "../aux/aux.h"
#include "../aux/point.h"
#include "../aux/curves.h"
#include "../xml/xml_parser.h"
using namespace std;

xml_parser parser;

/**
 * @brief Vector3 class for 3D point and vector operations
 */
class Vector3 {
public:
    float x, y, z;

    /**
     * @brief Constructor for Vector3
     * 
     * @param _x The x coordinate (default 0.0f)
     * @param _y The y coordinate (default 0.0f)
     * @param _z The z coordinate (default 0.0f)
     */
    Vector3(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) : x(_x), y(_y), z(_z) {}

    /**
     * @brief Vector addition operator
     * 
     * @param v The vector to add
     * @return Vector3 A new vector representing the sum
     */
    Vector3 operator+(const Vector3& v) const { 
        return Vector3(x + v.x, y + v.y, z + v.z); 
    }
    
    /**
     * @brief Vector subtraction operator
     * 
     * @param v The vector to subtract
     * @return Vector3 A new vector representing the difference
     */
    Vector3 operator-(const Vector3& v) const { 
        return Vector3(x - v.x, y - v.y, z - v.z); 
    }
    
    /**
     * @brief Scalar multiplication operator
     * 
     * @param s The scalar value to multiply by
     * @return Vector3 A new scaled vector
     */
    Vector3 operator*(float s) const { 
        return Vector3(x * s, y * s, z * s); 
    }
    
    /**
     * @brief Calculates the length of the vector
     * 
     * @return float The vector's magnitude
     */
    float length() const { 
        return sqrt(x * x + y * y + z * z); 
    }
    
    /**
     * @brief Calculates the distance between this vector and another
     * 
     * @param v The other vector
     * @return float The Euclidean distance between vectors
     */
    float distance(const Vector3& v) const {
        float dx = x - v.x;
        float dy = y - v.y;
        float dz = z - v.z;
        return sqrt(dx * dx + dy * dy + dz * dz);
    }
    
    /**
     * @brief Normalizes the vector to unit length
     * 
     * @return Vector3 A new unit vector in the same direction
     */
    Vector3 normalize() const {
        float len = length();
        if (len < 0.00001f) return Vector3();
        return Vector3(x / len, y / len, z / len);
    }
    
    /**
     * @brief Calculates the cross product with another vector
     * 
     * @param v The other vector
     * @return Vector3 The cross product vector
     */
    Vector3 cross(const Vector3& v) const {
        return Vector3(
            y * v.z - z * v.y,
            z * v.x - x * v.z,
            x * v.y - y * v.x
        );
    }
    
    /**
     * @brief Calculates the dot product with another vector
     * 
     * @param v The other vector
     * @return float The dot product value
     */
    float dot(const Vector3& v) const {
        return x * v.x + y * v.y + z * v.z;
    }
};

/**
 * @brief Converts degrees to radians
 * 
 * @param degrees The angle in degrees
 * @return float The angle in radians
 */
float radians(float degrees) { return degrees * M_PI / 180.0f; }

/**
 * @brief Converts radians to degrees
 * 
 * @param radians The angle in radians
 * @return float The angle in degrees
 */
float degrees(float radians) { return radians * 180.0f / M_PI; }

/**
 * @brief Camera system supporting both orbit and first-person modes
 */
struct Camera {
    enum Mode { ORBIT, FPS };
    Mode mode = ORBIT;
    
    // Common parameters
    float sensitivity = 0.1f;
    float move_speed = 0.5f;
    
    // Orbit mode parameters
    float orbit_alpha = 0.0f;
    float orbit_beta = 0.0f;
    float orbit_radius = 5.0f;
    Vector3 orbit_look_at;
    
    // FPS mode parameters
    Vector3 fps_position;
    Vector3 fps_front;
    Vector3 fps_right;
    Vector3 fps_up;
    float fps_yaw = -90.0f;
    float fps_pitch = 0.0f;
    
    /**
     * @brief Initializes camera parameters from XML configuration
     */
    void init_from_xml() {
        // Orbit mode initialization
        orbit_look_at = Vector3(parser.cam.lx, parser.cam.ly, parser.cam.lz);
        Vector3 initial_pos(parser.cam.px, parser.cam.py, parser.cam.pz);
        orbit_radius = initial_pos.distance(orbit_look_at);
        
        Vector3 dir = orbit_look_at - initial_pos;
        dir = dir.normalize();
        orbit_alpha = atan2(-dir.x, -dir.z);
        orbit_beta = -asin(dir.y);
        
        // FPS mode initialization
        fps_position = initial_pos;
        update_fps_vectors();
    }
    
    /**
     * @brief Updates the front, right, and up vectors for FPS mode
     */
    void update_fps_vectors() {
        Vector3 front;
        front.x = cos(radians(fps_yaw)) * cos(radians(fps_pitch));
        front.y = sin(radians(fps_pitch));
        front.z = sin(radians(fps_yaw)) * cos(radians(fps_pitch));
        fps_front = front.normalize();
        
        fps_right = fps_front.cross(Vector3(0.0f, 1.0f, 0.0f)).normalize();
        fps_up = fps_right.cross(fps_front).normalize();
    }
    
    /**
     * @brief Toggles between orbit and FPS camera modes
     */
    void toggle_mode() {
        if(mode == ORBIT) {
            // Switch to FPS - maintain current position
            mode = FPS;
        } else {
            // Switch to Orbit - update orbit parameters based on FPS position
            mode = ORBIT;
            orbit_radius = fps_position.distance(orbit_look_at);
            Vector3 dir = orbit_look_at - fps_position;
            dir = dir.normalize();
            orbit_alpha = atan2(-dir.x, -dir.z);
            orbit_beta = -asin(dir.y);
        }
    }
};

Camera camera;

// Track keys
bool keys[256] = {false}; // Track regular keys
bool special_keys[256] = {false}; // Track special keys

/**
 * @brief Mouse handling functionality for camera control
 */
int last_mouse_x = 0, last_mouse_y = 0;
bool mouse_dragging = false;

/**
 * @brief Processes mouse movement for camera control
 * 
 * @param x The current x-coordinate of the mouse pointer
 * @param y The current y-coordinate of the mouse pointer
 */
void process_mouse_motion(int x, int y) {
    if(camera.mode == Camera::FPS && mouse_dragging) {
        float xoffset = x - last_mouse_x;
        float yoffset = last_mouse_y - y;  // Reversed since y-coordinates go from bottom to top
        
        xoffset *= camera.sensitivity;
        yoffset *= camera.sensitivity;
        
        camera.fps_yaw += xoffset;
        camera.fps_pitch += yoffset;
        
        // Constrain pitch to avoid screen flip
        camera.fps_pitch = camera.fps_pitch < -89.0f ? -89.0f : (camera.fps_pitch > 89.0f ? 89.0f : camera.fps_pitch);
        
        camera.update_fps_vectors();
    }
    last_mouse_x = x;
    last_mouse_y = y;
}

/**
 * @brief Processes mouse button events for camera control
 * 
 * @param button The mouse button that was pressed or released
 * @param state The state of the button (GLUT_DOWN or GLUT_UP)
 * @param x The x-coordinate where the button was pressed/released
 * @param y The y-coordinate where the button was pressed/released
 */
void process_mouse_buttons(int button, int state, int x, int y) {
    if(button == GLUT_LEFT_BUTTON) {
        mouse_dragging = (state == GLUT_DOWN);
        last_mouse_x = x;
        last_mouse_y = y;
    }
}

/**
 * @brief Rendering parameters and state
 */
float scale = 1.0f;
int total_models = 0;

int last_frame_time = 0;

/**
 * @brief Class for storing Vertex Buffer Object (VBO) information
 */
class vbo {
public:
    GLuint vertices, total_vertices, indexes, total_indexes;
    
    /**
     * @brief Constructor for VBO objects
     * 
     * @param v The vertex buffer handle
     * @param tv Total number of vertices
     * @param i The index buffer handle
     * @param ti Total number of indices
     */
    vbo(GLuint v, unsigned int tv, GLuint i, GLuint ti) 
        : vertices(v), total_vertices(tv), indexes(i), total_indexes(ti) {}
};

unordered_map<string, vbo*> model_dict;

/**
 * @brief Handles window resize events and updates the projection matrix
 * 
 * @param w The new window width
 * @param h The new window height
 */
void change_size(int w, int h) {
    if(h == 0) h = 1;
    float ratio = w * 1.0 / h;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, w, h);
    gluPerspective(parser.cam.fov, ratio, parser.cam.near, parser.cam.far);
    glMatrixMode(GL_MODELVIEW);
}

/**
 * @brief Processes time-based translations for object animations
 * 
 * @param translation The translation parameters from XML
 */
void time_translation(translation_xml translation) {
    double t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
    t /= translation.time;
    t -= floor(t);  // Loop animation

    int tessellation = 100;
    int num_curve_points = (tessellation + 1) * translation.time_trans.points.size();
    float* curve_points = new float[num_curve_points * 3];
    float* div = new float[num_curve_points * 3];

    catmullrom_curve(tessellation, translation.time_trans.points, curve_points, div);

    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < num_curve_points; ++i) {
        int idx = i * 3;
        glVertex3f(curve_points[idx], curve_points[idx + 1], curve_points[idx + 2]);
    }
    glEnd();

    int current_index = static_cast<int>(t * num_curve_points);
    current_index = current_index % num_curve_points;

    int offset = current_index * 3;
    float px = curve_points[offset];
    float py = curve_points[offset + 1];
    float pz = curve_points[offset + 2];

    float current_div[3];
    current_div[0] = div[offset];
    current_div[1] = div[offset + 1];
    current_div[2] = div[offset + 2];

    glTranslatef(px, py, pz);

    if (translation.time_trans.align == 0) {
        static float y[4] = { 0.0f, 1.0f, 0.0f };
        float m[16];
        generate_catmull_matrix(current_div, y, m);
        glMultMatrixf(m);
    }

    delete[] div;
    delete[] curve_points;
}

/**
 * @brief Recursively draws a group and all its children
 * 
 * @param group The group to draw
 */
void recursive_draw(const group_xml& group) {
    glPushMatrix();

    int order[3] = {0};
    for(int i = 0; i < 3; i++) {
        if(group.transformations.rotation_exists && group.transformations.rotation.order == i)
            order[i] = 1;
        if(group.transformations.translation_exists && group.transformations.translation.order == i)
            order[i] = 2;
        if(group.transformations.scale_exists && group.transformations.scale.order == i)
            order[i] = 3;
    }

    for(int i = 0; i < 3; i++) {
        switch(order[i]) {
            case 1: // Rotation
                glRotatef(group.transformations.rotation.angle,
                        group.transformations.rotation.x,
                        group.transformations.rotation.y,
                        group.transformations.rotation.z);
                break;
            case 2: // Translation
                if(group.transformations.translation.time == 0)
                    glTranslatef(group.transformations.translation.x,
                                group.transformations.translation.y,
                                group.transformations.translation.z);
                else
                    time_translation(group.transformations.translation);
                break;
            case 3: // Scale
                glScalef(group.transformations.scale.x,
                        group.transformations.scale.y,
                        group.transformations.scale.z);
                break;
        }
    }

    glEnableClientState(GL_VERTEX_ARRAY);
    glColor3f(1.0f, 1.0f, 1.0f);

    for(const auto& model : group.models) {
        vbo* current_vbo = model_dict[model.file_name];
        total_models++;

        glBindBuffer(GL_ARRAY_BUFFER, current_vbo->vertices);
        glVertexPointer(3, GL_FLOAT, 0, 0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, current_vbo->indexes);
        glDrawElements(GL_TRIANGLES, current_vbo->total_indexes, GL_UNSIGNED_INT, 0);
    }

    glDisableClientState(GL_VERTEX_ARRAY);

    for(const auto& subgroup : group.groups)
        recursive_draw(subgroup);

    glPopMatrix();
}

/**
 * @brief Initializes camera parameters from XML data
 */
void init_camera() {
    camera.init_from_xml();
}

/**
 * @brief Updates camera position and orientation based on user input
 */
void update_camera() {
    if(camera.mode == Camera::ORBIT) {
        // Original orbit controls
        if(keys[(int)'a']) camera.orbit_alpha -= 0.01f;
        if(keys[(int)'d']) camera.orbit_alpha += 0.01f;
        if(keys[(int)'w']) camera.orbit_beta = std::min(camera.orbit_beta + 0.01f, float(M_PI/2 - 0.01));
        if(keys[(int)'s']) camera.orbit_beta = std::max(camera.orbit_beta - 0.01f, float(-M_PI/2 + 0.01));
        if(special_keys[GLUT_KEY_UP]) camera.orbit_radius = std::max(camera.orbit_radius - 0.25f, 1.0f);
        if(special_keys[GLUT_KEY_DOWN]) camera.orbit_radius += 0.25f;
    } else { // FPS mode
        // FPS movement controls
        float velocity = camera.move_speed;
        if(keys[(int)'w']) camera.fps_position = camera.fps_position + camera.fps_front * velocity;
        if(keys[(int)'s']) camera.fps_position = camera.fps_position - camera.fps_front * velocity;
        if(keys[(int)'a']) camera.fps_position = camera.fps_position - camera.fps_right * velocity;
        if(keys[(int)'d']) camera.fps_position = camera.fps_position + camera.fps_right * velocity;
        if(keys[(int)' ']) camera.fps_position.y += velocity;  // Space to ascend
        if(keys[(int)'c']) camera.fps_position.y -= velocity;  // C to descend
    }
    
    // Common controls
    if(keys[(int)'i']) scale += 0.025f;
    if(keys[(int)'o']) scale = std::max(scale - 0.025f, 0.05f);
}

/**
 * @brief Renders the scene with all models and visual elements
 */
void render_scene(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Set up view matrix based on camera mode
    if(camera.mode == Camera::ORBIT) {
        // Calculate orbit position
        float eye_x = camera.orbit_look_at.x + camera.orbit_radius * cos(camera.orbit_beta) * sin(camera.orbit_alpha);
        float eye_y = camera.orbit_look_at.y + camera.orbit_radius * sin(camera.orbit_beta);
        float eye_z = camera.orbit_look_at.z + camera.orbit_radius * cos(camera.orbit_beta) * cos(camera.orbit_alpha);
        
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

    glScalef(scale, scale, scale);
    for(const auto& group : parser.groups)
        recursive_draw(group);

    glutSwapBuffers();
}

/**
 * @brief Handles keyboard input for regular keys
 * 
 * @param key The key that was pressed
 * @param x The x-coordinate of the mouse when the key was pressed
 * @param y The y-coordinate of the mouse when the key was pressed
 */
void default_key_func(unsigned char key, int x, int y) {
    (void)x;
    (void)y;
    keys[tolower(key)] = true;
    
    if(tolower(key) == 'm') {  // Toggle camera mode with M key
        camera.toggle_mode();
        glutSetWindowTitle(camera.mode == Camera::ORBIT ? 
            "CG@33 - Orbit Mode" : "CG@33 - FPS Mode");
    }
    
    // Keep the 'q' exit action
    if(tolower(key) == 'q') {
        // Clean up resources before exiting
        for(auto& entry : model_dict) {
            glDeleteBuffers(1, &entry.second->vertices);
            glDeleteBuffers(1, &entry.second->indexes);
            delete entry.second;
        }
        exit(0);
    }
    
    glutPostRedisplay();
}

/**
 * @brief Handles keyboard release for regular keys
 * 
 * @param key The key that was released
 * @param x The x-coordinate of the mouse when the key was released
 * @param y The y-coordinate of the mouse when the key was released
 */
void default_key_up_func(unsigned char key, int x, int y) {
    (void)x;
    (void)y;
    keys[tolower(key)] = false;
}

/**
 * @brief Handles keyboard input for special keys (arrows, etc.)
 * 
 * @param key The special key that was pressed
 * @param x The x-coordinate of the mouse when the key was pressed
 * @param y The y-coordinate of the mouse when the key was pressed
 */
void special_key_func(int key, int x, int y) {
    (void)x;
    (void)y;
    special_keys[key] = true;
    glutPostRedisplay();
}

/**
 * @brief Handles keyboard release for special keys
 * 
 * @param key The special key that was released
 * @param x The x-coordinate of the mouse when the key was released
 * @param y The y-coordinate of the mouse when the key was released
 */
void special_key_up_func(int key, int x, int y) {
    (void)x;
    (void)y;
    special_keys[key] = false;
}

/**
 * @brief Loads models into VBOs and populates the model dictionary
 * 
 * @param group The group containing models to load
 * @param dict The dictionary to store model VBOs
 * @return int 0 on success, 1 on failure
 */
int populate_dict(const group_xml& group, unordered_map<string, vbo*>& dict) {
    for(const auto& model : group.models) {
        if(dict.find(model.file_name) == dict.end()) {
            vector<float> vertices;
            vector<unsigned int> indexes;
            
            if(read_model(model.file_name, vertices, indexes)) {
                cerr << "Error loading model: " << model.file_name << endl;
                return 1;
            }

            GLuint vboID, iboID;
            glGenBuffers(1, &vboID);
            glBindBuffer(GL_ARRAY_BUFFER, vboID);
            glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), vertices.data(), GL_STATIC_DRAW);

            glGenBuffers(1, &iboID);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexes.size()*sizeof(unsigned int), indexes.data(), GL_STATIC_DRAW);

            dict[model.file_name] = new vbo(vboID, vertices.size()/3, iboID, indexes.size());
        }
    }

    for(const auto& subgroup : group.groups)
        if(populate_dict(subgroup, dict)) return 1;

    return 0;
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
    glutReshapeFunc(change_size);

    glutKeyboardFunc(default_key_func);
    glutKeyboardUpFunc(default_key_up_func);
    glutSpecialFunc(special_key_func);
    glutSpecialUpFunc(special_key_up_func);
    
    glutMotionFunc(process_mouse_motion);
    glutMouseFunc(process_mouse_buttons);
    glutTimerFunc(0, timer_func, 0);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    if(populate_dict(parser.groups[0], model_dict)) {
        cerr << "Failed to load models\n";
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