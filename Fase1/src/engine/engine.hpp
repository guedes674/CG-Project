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

using namespace std;
using namespace tinyxml2;

// variáveis globais
extern int w_width, w_height;                          // Tamanho da janela lido do XML
extern float pos_x, pos_y, pos_z;                       // posição da câmera
extern float look_x, look_y, look_z;                    // orientação da câmera
extern float up_x, up_y, up_z;                          // eixo vertical da câmera
extern float fov, near, far;                            // perspectiva da câmera
extern float zoomx, zoomy, zoomz;                       // zoom da câmera
extern float cradius;                                  // raio da câmera
extern int mode, face;                                 // modo de visualização
extern vector<Figure> models;                          // modelos carregados a partir do XML

// Função auxiliar para ler um valor float a partir de um atributo
float getFloatAttribute(XMLElement* element, const char* attributeName, float defaultValue);

// Função auxiliar para ler um valor inteiro a partir de um atributo
int getIntAttribute(XMLElement* element, const char* attributeName, int defaultValue);

// Função auxiliar para carregar a configuração da câmera
void loadCameraConfiguration(XMLElement* pCamera);

// Função auxiliar para carregar as configurações da janela a partir do XML
void loadWindowConfiguration(XMLElement* pWindow);

// Função auxiliar para carregar os modelos 3D a partir do XML
void loadModels(XMLElement* pModels);

// Função para processar o arquivo XML
void xml_parser(const char* filename);

// Função para mudar o tamanho da janela
void changeSize(int w, int h);

// Função para processar as teclas normais
void processKeys(unsigned char key, int xx, int yy);

// Função para processar as teclas especiais
void processSpecialKeys(int key, int xx, int yy);

// Função para desenhar os modelos 3D carregados
void drawScene();

// Função de renderização principal
void renderScene(void);

#endif