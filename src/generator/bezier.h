#ifndef GENERATOR_BEZIER_H
#define GENERATOR_BEZIER_H

#include <vector>
#include <iostream>
#include <fstream>
#include "../aux/point.h"

void bezier(char* patch, int tessellation, std::vector<float>&vertices, std::vector<unsigned int>& indexes, std::vector<float>& normals, std::vector<float>& textures);
#endif