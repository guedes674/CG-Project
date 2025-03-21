#ifndef ENGINE
#define ENGINE

#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include <cmath>
#include <iostream>
#include <vector>
#include <string>
#include "tinyxml2.h"
#include "../classes/figure.hpp"
#include "../classes/point.hpp"
#include "../classes/list.hpp"

using namespace std;
using namespace tinyxml2;

// vari�veis globais
extern int w_width, w_height;                          // Tamanho da janela lido do XML
extern float pos_x, pos_y, pos_z;                       // posi��o da c�mera
extern float look_x, look_y, look_z;                    // orienta��o da c�mera
extern float up_x, up_y, up_z;                          // eixo vertical da c�mera
extern float fov, near, far;                            // perspectiva da c�mera
extern float zoomx, zoomy, zoomz;                       // zoom da c�mera
extern float cradius;                                  // raio da c�mera
extern int mode, face;                                 // modo de visualiza��o
extern vector<Figure> models;                          // modelos carregados a partir do XML

// Fun��o auxiliar para ler um valor float a partir de um atributo
float getFloatAttribute(XMLElement* element, const char* attributeName, float defaultValue);

// Fun��o auxiliar para ler um valor inteiro a partir de um atributo
int getIntAttribute(XMLElement* element, const char* attributeName, int defaultValue);

// Fun��o auxiliar para carregar a configura��o da c�mera
void loadCameraConfiguration(XMLElement* pCamera);

// Fun��o auxiliar para carregar as configura��es da janela a partir do XML
void loadWindowConfiguration(XMLElement* pWindow);

// Fun��o auxiliar para carregar os modelos 3D a partir do XML
void loadModels(XMLElement* pModels);

// Fun��o para processar o arquivo XML
void xml_parser(const char* filename);

// Fun��o para mudar o tamanho da janela
void changeSize(int w, int h);

// Fun��o para processar as teclas normais
void processKeys(unsigned char key, int xx, int yy);

// Fun��o para processar as teclas especiais
void processSpecialKeys(int key, int xx, int yy);

// Fun��o para desenhar os modelos 3D carregados
void drawScene();

// Fun��o de renderiza��o principal
void renderScene(void);

#endif