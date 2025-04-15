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
int total_models = 0;
xml_parser parser;

class vbo{
	public:
		GLuint vertices, total_vertices, indexes, total_indexes;

        vbo(GLuint v, unsigned int tv, GLuint i, GLuint ti) : vertices(v), total_vertices(tv), indexes(i), total_indexes(ti){};

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

/*
//Retorna a Model View Matrix
static inline void mvp_matrix(float* matrix){
	float modelview_matrix[16],projection_matrix[16];
	glGetFloatv(GL_MODELVIEW_MATRIX,modelview_matrix);
	glGetFloatv(GL_PROJECTION_MATRIX,projection_matrix);

	glPushMatrix();
	glLoadMatrixf(projection_matrix);
	glMultMatrixf(modelview_matrix);
	glGetFloatv(GL_MODELVIEW_MATRIX,matrix);
	glPopMatrix();
}
*/

void recursive_draw(const group_xml& group) {
    glPushMatrix();

	int transformation_order[3] = {0,0,0};

	for(int i = 0; i < 3;i++){
		if(group.transformations.rotation_exists == 1){
			if(group.transformations.rotation.order == i){
				transformation_order[i] = 2;
			}
		}
		if (group.transformations.translation_exists ==1){
			if(group.transformations.translation.order == i){
				transformation_order[i] = 1;
			}
		}
		if (group.transformations.scale_exists == 1){
			if(group.transformations.scale.order == i){
				transformation_order[i] = 3;
			}
		}
	}
	
	for(int i = 0; i < 3;i++){
		if (transformation_order[i]!= 0){
			switch(transformation_order[i]) {
				case 1: // Translation
					glTranslatef(group.transformations.translation.x,
								group.transformations.translation.y,
								group.transformations.translation.z);
					break;
				case 2: // Rotation
					glRotatef(group.transformations.rotation.angle,
							group.transformations.rotation.x,
							group.transformations.rotation.y,
							group.transformations.rotation.z);
					break;
				case 3: // Scale
					glScalef(group.transformations.scale.x,
							group.transformations.scale.y,
							group.transformations.scale.z);
					break;
			}
		}
	}

    // Enable necessary client states
    glEnableClientState(GL_VERTEX_ARRAY);
    // glEnableClientState(GL_NORMAL_ARRAY);  // Uncomment if you have normal data

	// Set color to white for all subsequent rendering
	glColor3f(1.0f, 1.0f, 1.0f);

    for (int i = 0; static_cast<unsigned long>(i) < group.models.size(); i++) {
        vbo * current_vbo = model_dict[group.models[i].file_name];
        total_models++;

        glBindBuffer(GL_ARRAY_BUFFER, current_vbo->vertices);
        glVertexPointer(3, GL_FLOAT, 0, 0);
        
        // If you have normal data, bind it properly
        // glBindBuffer(GL_ARRAY_BUFFER, current_vbo->normals);
        // glNormalPointer(GL_FLOAT, 0, 0);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, current_vbo->indexes);
        glDrawElements(GL_TRIANGLES, current_vbo->total_indexes, GL_UNSIGNED_INT, 0);
    }

    // Disable client states
    glDisableClientState(GL_VERTEX_ARRAY);
    // glDisableClientState(GL_NORMAL_ARRAY);  // Uncomment if you have normal data

    for (int i = 0; static_cast<unsigned long>(i) < group.groups.size(); i++)
        recursive_draw(group.groups[i]);

    glPopMatrix();
}

void renderScene(void) {

	// Clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw the object
	vector<float> vertices;
	vector<unsigned int> indexes;

	// Loading the identity matrix
	glLoadIdentity();

	// Set the camera position and orientation
	gluLookAt(parser.cam.px, parser.cam.py, parser.cam.pz,
		parser.cam.lx, parser.cam.ly, parser.cam.lz,
		parser.cam.ux, parser.cam.uy, parser.cam.uz);

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

	glRotatef(angle_y, 0.0f, 1.0f, 0.0f);
	glRotatef(angle_x, 1.0f, 0.0f, 0.0f);
	glRotatef(angle_z, 0.0f, 0.0f, 1.0f);
	glTranslatef(xx, 0.0f, zz);
	glScalef(scale, scale, scale);
	
	for (int i = 0; static_cast<unsigned long>(i) < parser.groups.size(); i++){
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

	for(int i = 0; static_cast<unsigned long>(i) < group.models.size(); i++){
		// Checking if model is already in the dictionary
		if(model_dict.find(group.models[i].file_name) == model_dict.end()){
			vector<float> vertices;
			vector<unsigned int> indexes;
			if(read_model(group.models[i].file_name, vertices, indexes)){
				cout << "Error : File\"" << group.models[i].file_name << "\" not found \n";
				return 1;
			}
			else{
				GLuint vertices_id, indexes_id, total_vertices = vertices.size()/3, total_indexes = indexes.size();

				// Create vbo
				glGenBuffers(1, &vertices_id);

				//Copy vector to graphics card
				glBindBuffer(GL_ARRAY_BUFFER, vertices_id);
				glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

                //Create VBO indexes 
                glGenBuffers(1, &indexes_id);

                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexes_id);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indexes.size(), indexes.data(), GL_STATIC_DRAW);

				auto new_vbo = new vbo(vertices_id, total_vertices, indexes_id, total_indexes);
				model_dict[group.models[i].file_name] = new_vbo;
			}
		}
	}
	for(int i = 0; static_cast<unsigned long>(i) < group.groups.size(); i++){
		if (populate_dict(group.groups[i], model_dict) == 1) {
			cout << "Error populating dictionary for group" <<
			i 
			<< endl;
			return 1;
		}
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
	glutCreateWindow("CG@33");

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

	if (populate_dict(parser.groups[0], model_dict)) {
		cout << "Error populating dictionary" << endl;
		return 1;
	}

	// enter GLUT's main cycle
	glutMainLoop();

	return 1;
}