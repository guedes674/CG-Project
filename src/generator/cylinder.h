#ifndef GENERATOR_CYLINDER_H
#define GENERATOR_CYLINDER_H

#include <vector>
#include <cmath>

void cylinder(float radius, float height, int slices, int stacks, std::vector<float>& vertices, std::vector<unsigned int>& indexes);

#endif