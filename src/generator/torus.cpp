#include "torus.h"
using namespace std;

void torus(float inner_radius, float outer_radius, int slices, int stacks, vector<float>& vertices, vector<unsigned int>& indexes, vector<float>& normals, vector<float>& textures) {
    float radius = inner_radius + outer_radius;
    float slices_ang_inc = 2 * M_PI / slices;
    float stacks_ang_inc = 2 * M_PI / stacks;
    float radius_x, radius_z, angle, aux_radius, cos_B, sin_B;

    vertices.reserve(slices * stacks * 3);

    for (int i = 0; i <= slices; i++) {

        angle = slices_ang_inc * i;

        cos_B = cos(angle);
        sin_B = sin(angle);

        radius_x = cos_B * radius;
        radius_z = sin_B * radius;

        for (int j = 0; j <= stacks; j++) {

            aux_radius = cos(stacks_ang_inc * j) * inner_radius;
            vertices.push_back(radius_x + aux_radius * cos_B);
            vertices.push_back(sin(stacks_ang_inc * j) * inner_radius);
            vertices.push_back(radius_z + aux_radius * sin_B);

            normals.push_back(cos(stacks_ang_inc * j) * cos_B);
            normals.push_back(sin(stacks_ang_inc * j));
            normals.push_back(cos(stacks_ang_inc * j) * sin_B);

            textures.push_back(float(i)/float(slices));
            textures.push_back(float(j)/float(stacks));
            
        }
    }

    for (int i = 0; i < slices; i++) {
        for (int j = 0; j < stacks; j++) {
            indexes.push_back(i*(stacks+1)+j);  
            indexes.push_back(i*(stacks+1)+(j + 1));
            indexes.push_back((i+1)*(stacks+1)+j);

            indexes.push_back(i*(stacks+1)+(j + 1));
            indexes.push_back((i+1)*(stacks+1)+(j+1));
            indexes.push_back((i+1)*(stacks+1)+j);
        }
    }
}