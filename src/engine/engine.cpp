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
#include "../generator/model.h"
#include "../aux/aux.h"
#include "../aux/point.h"
#include "../aux/curves.h"
#include "../xml/xml_parser.h"
using namespace std;

// Camera system
bool firstPersonMode = false;
float cam_radius;
float cam_alpha;
float cam_beta;

// Add these near your other global variables
bool keys[256] = {false}; // Track regular keys
bool specialKeys[256] = {false}; // Track special keys

// Vector math struct and functions
struct vec3 { 
    float x, y, z; 
    vec3 operator+(vec3 b) { return {x+b.x, y+b.y, z+b.z}; }
    vec3 operator-(vec3 b) { return {x-b.x, y-b.y, z-b.z}; }
    vec3 operator*(float s) { return {x*s, y*s, z*s}; }
};

vec3 cross_product(vec3 a, vec3 b) {
    return {
        a.y*b.z - a.z*b.y,
        a.z*b.x - a.x*b.z,
        a.x*b.y - a.y*b.x
    };
}

vec3 normalize(vec3 v) {
    float len = sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
    if(len == 0) return {0,0,0};
    return {v.x/len, v.y/len, v.z/len};
}

float radians(float degrees) { return degrees * M_PI / 180.0f; }
float degrees(float radians) { return radians * 180.0f / M_PI; }

// First-person camera
vec3 fp_pos;
float fp_yaw = -90.0f;
float fp_pitch = 0.0f;
float lastX = 400, lastY = 400;
bool firstMouse = true;
float cameraSpeed = 0.1f;

// Add a global variable for the front vector
vec3 fp_front;

// Rendering
float scale = 1.0f;
int total_models = 0;
xml_parser parser;

class vbo {
public:
    GLuint vertices, total_vertices, indexes, total_indexes;
    vbo(GLuint v, unsigned int tv, GLuint i, GLuint ti) 
        : vertices(v), total_vertices(tv), indexes(i), total_indexes(ti) {}
};

unordered_map<string, vbo*> model_dict;

void changeSize(int w, int h) {
    if(h == 0) h = 1;
    float ratio = w * 1.0 / h;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, w, h);
    gluPerspective(parser.cam.fov, ratio, parser.cam.near, parser.cam.far);
    glMatrixMode(GL_MODELVIEW);
}

void mouseMotion(int x, int y) {
    if(firstPersonMode) {
        if(firstMouse) {
            lastX = x;
            lastY = y;
            firstMouse = false;
        }

        float xoffset = x - lastX;
        float yoffset = lastY - y; 
        lastX = x;
        lastY = y;

        float sensitivity = 0.1f;
        fp_yaw += xoffset * sensitivity;
        fp_pitch += yoffset * sensitivity;

        if(fp_pitch > 89.0f) fp_pitch = 89.0f;
        if(fp_pitch < -89.0f) fp_pitch = -89.0f;

        glutWarpPointer(400, 400);
        lastX = lastY = 400;
    }
}

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

    if (translation.time_trans.align) {
        static float y[4] = { 0.0f, 1.0f, 0.0f };
        float m[16];
        generate_catmull_matrix(current_div, y, m);
        glMultMatrixf(m);
    }

    delete[] div;
    delete[] curve_points;
}

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

void initCamera() {
    float dx = parser.cam.px - parser.cam.lx;
    float dy = parser.cam.py - parser.cam.ly;
    float dz = parser.cam.pz - parser.cam.lz;

    cam_radius = sqrt(dx*dx + dy*dy + dz*dz);
    cam_alpha = atan2(dx, dz);
    cam_beta = asin(dy / cam_radius);

    fp_pos = {parser.cam.px, parser.cam.py, parser.cam.pz};
    fp_yaw = -90.0f + degrees(atan2(dz, dx));
    fp_pitch = degrees(asin(dy / cam_radius));
}

void defaultKeyFunc(unsigned char key, int x, int y) {
    keys[tolower(key)] = true;
    
    // Keep the 'q' exit and 'c' mode toggle as immediate actions
    if(tolower(key) == 'q') {
        // Clean up resources before exiting
        for(auto& entry : model_dict) {
            glDeleteBuffers(1, &entry.second->vertices);
            glDeleteBuffers(1, &entry.second->indexes);
            delete entry.second;
        }
        exit(0);
    } else if(tolower(key) == 'c') {
        firstPersonMode = !firstPersonMode;
        glutSetCursor(firstPersonMode ? GLUT_CURSOR_NONE : GLUT_CURSOR_INHERIT);
        if(firstPersonMode) glutWarpPointer(400, 400);
    }
    
    glutPostRedisplay();
}

void defaultKeyUpFunc(unsigned char key, int x, int y) {
    keys[tolower(key)] = false;
}

void specialKeyFunc(int key, int x, int y) {
    specialKeys[key] = true;
    glutPostRedisplay();
}

void specialKeyUpFunc(int key, int x, int y) {
    specialKeys[key] = false;
}

void updateCamera() {
    if(firstPersonMode) {
        // Calculate front and right vectors
        fp_front = {
            cos(radians(fp_yaw)) * cos(radians(fp_pitch)),
            sin(radians(fp_pitch)),
            sin(radians(fp_yaw)) * cos(radians(fp_pitch))
        };
        fp_front = normalize(fp_front);
        
        vec3 right = cross_product(fp_front, {0.0f, 1.0f, 0.0f});
        right = normalize(right);
        
        // Apply movement based on which keys are pressed
        if(keys['w']) fp_pos = fp_pos + fp_front * cameraSpeed;
        if(keys['s']) fp_pos = fp_pos - fp_front * cameraSpeed;
        if(keys['a']) fp_pos = fp_pos - right * cameraSpeed;
        if(keys['d']) fp_pos = fp_pos + right * cameraSpeed;
    } else {
        // Third-person camera updates
        if(keys['a']) cam_alpha -= 0.01f;
        if(keys['d']) cam_alpha += 0.01f;
        if(keys['w']) cam_beta = min(cam_beta + 0.01f, float(M_PI/2 - 0.01));
        if(keys['s']) cam_beta = max(cam_beta - 0.01f, float(-M_PI/2 + 0.01));
        if(keys['i']) scale += 0.025f;
        if(keys['o']) scale = max(scale - 0.025f, 0.05f);
        if(specialKeys[GLUT_KEY_UP]) cam_radius = max(cam_radius - 0.25f, 1.0f);
        if(specialKeys[GLUT_KEY_DOWN]) cam_radius += 0.25f;
    }
}

void renderScene(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    if(firstPersonMode) {

        // Use the precalculated front vector instead of recalculating
        gluLookAt(fp_pos.x, fp_pos.y, fp_pos.z,
                fp_pos.x + fp_front.x,
				fp_pos.y + fp_front.y,
                fp_pos.z + fp_front.z,
                0.0f, 1.0f, 0.0f);
    } else {
        float eyeX = parser.cam.lx + cam_radius * cos(cam_beta) * sin(cam_alpha);
        float eyeY = parser.cam.ly + cam_radius * sin(cam_beta);
        float eyeZ = parser.cam.lz + cam_radius * cos(cam_beta) * cos(cam_alpha);

        gluLookAt(eyeX, eyeY, eyeZ,
                parser.cam.lx, parser.cam.ly, parser.cam.lz,
                parser.cam.ux, parser.cam.uy, parser.cam.uz);
    }

		// --- Draw the axes ---
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

void timerFunc(int value) {
    updateCamera();  // Process movement based on key states
    glutPostRedisplay();
    glutTimerFunc(8, timerFunc, 0);  // 8ms for ~120 FPS (smoother)
}

int main(int argc, char** argv) {
    if(argc != 2) {
        cerr << "Usage: " << argv[0] << " <scene.xml>\n";
        return 1;
    }

    parser = read_xml_file(argv[1]);
    initCamera();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(800, 800);
    glutCreateWindow("3D Scene Viewer");

    #ifndef __APPLE__
    // Only initialize GLEW on non-Apple platforms (Linux/Windows)
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        cerr << "Error initializing GLEW: " << glewGetErrorString(err) << endl;
        return 1;
    }
    #endif

    glutDisplayFunc(renderScene);
    glutReshapeFunc(changeSize);
    glutKeyboardFunc(defaultKeyFunc);
    glutKeyboardUpFunc(defaultKeyUpFunc);
    glutSpecialFunc(specialKeyFunc);
    glutSpecialUpFunc(specialKeyUpFunc);
    glutPassiveMotionFunc(mouseMotion);
    glutTimerFunc(0, timerFunc, 0);

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