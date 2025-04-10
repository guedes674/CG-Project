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
#include "../generator/model.h"
#include "../aux/aux.h"
using namespace std;

float angle_y = 0.0f;
float angle_x = 0.0f;
float angle_z = 0.0f;
float scale = 1.0f;
float xx = 0.0f, zz = 0.0f;
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
	gluPerspective(45.0f, ratio, 1.0f, 1000.0f);

	// return to the model view matrix mode
	glMatrixMode(GL_MODELVIEW);
}

void renderScene(void) {

	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	// set the camera
	glLoadIdentity();


	gluLookAt(5.0, 5.0, 5.0,
		0.0, 0.0, 0.0,
		0.0f, 1.0f, 0.0f);

	glBegin(GL_LINES);
	// X axis in red
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(100.0f, 0.0f, 0.0f);
	// Y Axis in Green
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 100.0f, 0.0f);
	// Z Axis in Blue
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 100.0f);
	glEnd();


	// put the geometric transformations here

	glRotatef(angle_y, 0.0f, 1.0f, 0.0f);
	glRotatef(angle_x, 1.0f, 0.0f, 0.0f);
	glRotatef(angle_z, 0.0f, 0.0f, 1.0f);
	glTranslatef(xx, 0.0f, zz);
	glScalef(scale, scale, scale);

	// Draw the object
	vector<float> vertices;
	vector<unsigned int> indices;

	// Read the model file
	string file_name = "sphere_1_10_10.3d";

	read_model(file_name, vertices, indices);
	
	for (int i = 0; static_cast<unsigned long>(i) < indices.size(); i += 3) {
		glBegin(GL_TRIANGLES);
		glColor3f(0.5f, 0.5f, 0.5f);
		glVertex3f(vertices[indices[i] * 3], vertices[indices[i] * 3 + 1], vertices[indices[i] * 3 + 2]);
		glVertex3f(vertices[indices[i + 1] * 3], vertices[indices[i + 1] * 3 + 1], vertices[indices[i + 1] * 3 + 2]);
		glVertex3f(vertices[indices[i + 2] * 3], vertices[indices[i + 2] * 3 + 1], vertices[indices[i + 2] * 3 + 2]);
		glEnd();
	}

	// End of frame
	glutSwapBuffers();
}



// write function to process keyboard events

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

int main(int argc, char** argv) {

	std::ifstream fd;
	string line;

	if (argc != 2) {
        cout << "Uso: " << argv[0] << " <arquivo_xml>" << endl;
        return 1;
    }

	

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