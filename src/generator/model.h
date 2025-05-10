#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <vector>
#include <algorithm>
#include <limits>
#include <fstream>
#include "../aux/aux.h"
#include "../aux/point.h"

int generate_model(std::string file_name, std::vector<float>& vertices, std::vector<unsigned int>& indexes, std::vector<float>& normals, std::vector<float>& textures);
int read_model(std::string file_name, std::vector<float>& vertices, std::vector<unsigned int>& indexes,std::vector<float>& normals,std::vector<float>& textures,float * bounding_box , Vector3 center, float radius);

#endif