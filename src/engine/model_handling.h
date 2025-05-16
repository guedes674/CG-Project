#ifndef MODEL_HANDLING_H
#define MODEL_HANDLING_H

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/gl.h>
#endif
#include <IL/il.h>
#include <cfloat>    
#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
#include "../aux/aux.h"
#include "../aux/point.h"
#include "../xml/xml_parser.h"
#include "../aux/curves.h"
#include "../generator/model.h"

extern int current_models;
extern bool show_normals;
extern bool show_bounding_box;
extern bool show_catmull_curves;
extern bool snapshot;
extern int last_time;
extern int current_target_index;
extern float gl_last_matrix[16];
extern std::unordered_map<int, Vector3> position_dict;
extern std::vector<int> position_keys;
/**
 * @brief Class for storing Vertex Buffer Object (VBO) information
 */
class vbo {
    public:
        GLuint vertices, total_vertices, indexes, total_indexes,normals,textures;
        float bounding_box[6]; // xmin, xmax, ymin, ymax, zmin, zmax
        Vector3 center;
        float radius;
        /**
         * @brief Constructor for VBO objects
         * 
         * @param v The vertex buffer handle
         * @param tv Total number of vertices
         * @param i The index buffer handle
         * @param ti Total number of indices
         * @param bounding_box 
         */
        vbo(GLuint v, unsigned int tv, GLuint i, GLuint ti,GLuint n, GLuint t,float* bounding_box, Vector3 c, float r) 
        : vertices(v), total_vertices(tv), indexes(i), total_indexes(ti),normals(n), textures(t), center(c), radius(r) {
            for (int j = 0; j < 6; ++j) {
                this->bounding_box[j] = bounding_box[j];
            }
        }

        void recomputeAABB_fromVBO(const float model[16]);
};

extern std::unordered_map<std::string, vbo*> model_dict;
extern std::unordered_map<std::string, GLuint> texture_dict;
extern std::vector<vbo*> snapshot_models;

int populate_dict(const group_xml& group, unordered_map<string, vbo*>& dict, unordered_map<string, GLuint>& texture_dict);
void recursive_draw(const group_xml& group);

#endif