#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <vector>
#include <fstream>
#include "../aux/aux.h"

int generate_model(std::string file_name, std::vector<float>& vertices, std::vector<unsigned int>& indices);
int read_model(std::string file_name, std::vector<float>& vertices, std::vector<unsigned int>& indexes);

#endif