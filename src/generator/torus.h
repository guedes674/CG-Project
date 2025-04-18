#ifndef GENERATOR_TORUS_H
#define GENERATOR_TORUS_H

#include <cmath>
#include <vector>

void torus(float inner_radius, float outer_radius, int slices, int stacks, std::vector<float>& vertices, std::vector<unsigned int>& indexes);

#endif