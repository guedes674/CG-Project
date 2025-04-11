#include <stdlib.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
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
#include "../xml/xml_parser.h"
using namespace std;

float angle_y = 0.0f;
float angle_x = 0.0f;
float angle_z = 0.0f;
float scale = 1.0f;
float xx = 0.0f, zz = 0.0f;

xml_parser parser;

class vbo{
	public:
		GLuint vertices, total_vertices, indices;

        vbo(GLuint v, unsigned int tv, GLuint i) : vertices(v), total_vertices(tv), indices(i) {
        };

};

unordered_map <std::string,vbo*> model_dict;

void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window with zero width).
	if (h == 0)
		h = 1;

	// compute window's aspect ratio
	float ratio = w * 1.0 / h;

	// Set the projection matrix as current
	glMatrixMode(GL_PROJECTION);
	// Load Identity Matrix
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set perspective
	gluPerspective(parser.cam.fov, ratio, parser.cam.near, parser.cam.far);

	// return to the model view matrix mode
	glMatrixMode(GL_MODELVIEW);
}

// Função recursiva que desenha todos os modelos do grupo e de seus subgrupos.
void recursive_draw(const group_xml &group) {
    glPushMatrix();

	if(group.transformations.translation_exists == 1){
		glTranslatef(group.transformations.translation.x,
			group.transformations.translation.y,
			group.transformations.translation.z);
	}	
	if(group.transformations.rotation_exists == 1){
		glRotatef(group.transformations.rotation.angle,
			group.transformations.rotation.x,
			group.transformations.rotation.y,
			group.transformations.rotation.z);
	}
	if(group.transformations.scale_exists == 1){
		glScalef(group.transformations.scale.x,
			group.transformations.scale.y,
			group.transformations.scale.z);
	}
	
    // Draw the models for the current group.
    for (size_t j = 0; j < group.models.size(); j++) {
        vector<float> vertices;
        vector<unsigned int> indices;

        if (read_model(group.models[j].file_name, vertices, indices)) {
            std::cerr << "Error loading model: " << group.models[j].file_name << std::endl;
            continue;
        }

        for (size_t i = 0; i < indices.size(); i += 3) {
            glBegin(GL_TRIANGLES);
                glColor3f(0.5f, 0.5f, 0.5f);
                glVertex3f(vertices[indices[i] * 3],
                           vertices[indices[i] * 3 + 1],
                           vertices[indices[i] * 3 + 2]);
                glVertex3f(vertices[indices[i + 1] * 3],
                           vertices[indices[i + 1] * 3 + 1],
                           vertices[indices[i + 1] * 3 + 2]);
                glVertex3f(vertices[indices[i + 2] * 3],
                           vertices[indices[i + 2] * 3 + 1],
                           vertices[indices[i + 2] * 3 + 2]);
            glEnd();
        }
    }

    // Recursively draw all subgroups.
    for (size_t i = 0; i < group.groups.size(); i++) {
        recursive_draw(group.groups[i]);
    }

    glPopMatrix();
}

void renderScene(void) {

	// Clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw the object
	vector<float> vertices;
	vector<unsigned int> indices;

	// Loading the identity matrix
	glLoadIdentity();

	// Set the camera position and orientation
	gluLookAt(parser.cam.px, parser.cam.py, parser.cam.pz,
		parser.cam.lx, parser.cam.ly, parser.cam.lz,
		parser.cam.ux, parser.cam.uy, parser.cam.uz);

	// --- Draw the axes ---
	glBegin(GL_LINES);
		glColor3f(1.0f, 0.0f, 0.0f);   // x axis in red
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(100.0f, 0.0f, 0.0f);

		glColor3f(0.0f, 1.0f, 0.0f);   // y axis in green
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 100.0f, 0.0f);

		glColor3f(0.0f, 0.0f, 1.0f);   // z axis in blue
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, 100.0f);
	glEnd();

	glRotatef(angle_y, 0.0f, 1.0f, 0.0f);
	glRotatef(angle_x, 1.0f, 0.0f, 0.0f);
	glRotatef(angle_z, 0.0f, 0.0f, 1.0f);
	glTranslatef(xx, 0.0f, zz);
	glScalef(scale, scale, scale);

	for (size_t i = 0; i < parser.groups.size(); i++) {
		recursive_draw(parser.groups[i]);
	}
	// End of frame
	glutSwapBuffers();
}

void defaultKeyFunc(unsigned char key, int x, int y) {
	(void)x;  // Explicitly mark as unused
    (void)y;  // Explicitly mark as unused

	if (key == 'a' || key == 'A') {
		angle_y = angle_y + 5;
		glutPostRedisplay();
	}
	else if (key == 'd' || key == 'D') {
		angle_y = angle_y - 5;
		glutPostRedisplay();
	}
	else if (key == 'w' || key == 'W') {
		angle_x = angle_x + 5;
		glutPostRedisplay();
	}
	else if (key == 's' || key == 'S') {
		angle_x = angle_x - 5;
		glutPostRedisplay();
	}
	else if (key == 'i' || key == 'I') {
		scale += 0.05f;
		glutPostRedisplay();
	}
	else if (key == 'o' || key == 'O') {
		if (scale > 0.05f) {
			scale -= 0.05f;
			glutPostRedisplay();
		}
	}
}

void specialKeyFunc(int key_code, int x, int y) {
	(void)x;  // Explicitly mark as unused
    (void)y;  // Explicitly mark as unused

	if (key_code == GLUT_KEY_UP) {
		zz -= 0.1f;
		glutPostRedisplay();
	}
	else if (key_code == GLUT_KEY_DOWN) {
		zz += 0.1f;
		glutPostRedisplay();
	}
	else if (key_code == GLUT_KEY_RIGHT) {
		xx += 0.1f;
		glutPostRedisplay();
	}
	else if (key_code == GLUT_KEY_LEFT) {
		xx -= 0.1f;
		glutPostRedisplay();
	}
}

int populate_dict(group_xml group, unordered_map<std::string,vbo*>& model_dict){

	for(int i = 0; i < group.models.size(); i++){
		// Checking if model is already in the dictionary
		if(model_dict.find(group.models[i].file_name) == model_dict.end()){
			vector<float> vertices;
			vector<unsigned int> indices;
			if(read_model(group.models[i].file_name, vertices, indices)){
				cout << "Error : File\"" << group.models[i].file_name << "\" not found \n";
				return 1;
			}
			else{
				GLuint vertices_id, indices_id, total_vertices = vertices.size()/3;

				// Create vbo
				glGenBuffers(1, &vertices_id);

				//Copy vector to graphics card
				glBindBuffer(GL_ARRAY_BUFFER, vertices_id);
				glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

                //Create VBO indices 
                glGenBuffers(1, &indices_id);

                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

				auto new_vbo = new vbo(vertices_id, total_vertices, indices_id);
				model_dict[group.models[i].file_name] = new_vbo;
			}
		}
	}
	for(int i = 0; i < group.groups.size(); i++){
		if (populate_dict(group.groups[i], model_dict) == 1) return 1;
	}
	return 0;
}



int main(int argc, char** argv) {

	std::ifstream fd;
	string line;

	if (argc != 2) {
        cout << "Uso: " << argv[0] << " <arquivo_xml>" << endl;
        return 1;
    }

	parser = read_xml_file(argv[1]);

	// init GLUT and the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 800);
	glutCreateWindow("CG@DI-UM");

	// Required callback registry
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);


	// put here the registration of the keyboard callbacks
	glutKeyboardFunc(defaultKeyFunc);
	glutSpecialFunc(specialKeyFunc);


	//  OpenGL settings
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// enter GLUT's main cycle
	glutMainLoop();

	return 1;
}