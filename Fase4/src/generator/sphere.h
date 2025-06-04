#ifndef GENERATOR_SPHERE_H
#define GENERATOR_SPHERE_H

#include <vector>
#include <cmath>

void sphere(float radius, int slices, int stacks, std::vector<float>& vertices, std::vector<unsigned int>& indexes, std::vector<float>& normals, std::vector<float>& textures);

#endif