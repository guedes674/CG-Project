#ifndef GENERATOR_CONE_H
#define GENERATOR_CONE_H

#include <vector>
#include <cmath>

void cone(float base, float height, int slices, int stacks, std::vector<float>& vertices, std::vector<unsigned int>& indexes, std::vector<float>& normals, std::vector<float>& textures);
#endif