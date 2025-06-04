#ifndef XML_PARSER_H
#define XML_PARSER_H
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include <cmath>
#include <iostream>
#include <vector>
#include <string>

#include "camera.hpp"
#include "group.hpp"
#include "tinyxml2.h"
#include "../classes/figure.hpp"
#include "../classes/point.hpp"

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

class xml_parser {
  private:
    camera cam;
    group* grupo;
  public:

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

    camera getCamera();

    group* getGroup();

    void addModel(Figure model);

    void parseCamera(XMLElement * elem);

    void parseGroup(string path, XMLElement * elem, group *g);

    void parseModels(string path, XMLElement * elem, group * g);

    void parseTransform(XMLElement * elem, group *g);

    // Fun��o para processar o arquivo XML
    void xml_parser_function(const char* filename);
};


#endif