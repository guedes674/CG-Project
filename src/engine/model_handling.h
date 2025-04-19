#ifndef MODEL_HANDLING_H
#define MODEL_HANDLING_H

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/gl.h>
#endif

#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
#include "../aux/aux.h"
#include "../aux/point.h"
#include "../xml/xml_parser.h"
#include "../aux/curves.h"
#include "../generator/model.h"

extern int nmodels;
extern int total_models;
extern std::unordered_map<int, Vector3> position_dict;
extern std::vector<int> position_keys;
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

extern std::unordered_map<std::string, vbo*> model_dict;

int populate_dict(const group_xml& group, unordered_map<string, vbo*>& dict);
void recursive_draw(const group_xml& group);

#endif