#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <vector>
#include <algorithm>
#include <limits>
#include <fstream>
#include "../aux/aux.h"

int generate_model(std::string file_name, std::vector<float>& vertices, std::vector<unsigned int>& indexes);
int read_model(std::string file_name, std::vector<float>& vertices, std::vector<unsigned int>& indexes,float * bounding_box);

#endif