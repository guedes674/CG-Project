#include "box.h"

using namespace std;

void pushback_vertex(vector<float>& vertices, float x, float y, float z){
    vertices.push_back(x);
    vertices.push_back(y);
    vertices.push_back(z);
}

void box(float length, int divisions, vector<float>& vertices, vector<unsigned int>& indexes) {
    int face_vertices = (divisions + 1) * (divisions + 1);

    vertices.resize(6 * face_vertices * 3);

    float half_length = length / 2.0f;
    float x;
    float xi = x = -half_length;
    float z = -half_length;

    float box_side_size = length / divisions;

    float y_t = box_side_size * divisions - half_length;
    float y_b = -half_length;

    float vertices_per_line = divisions + 1;

    for(int i = 0; i<= divisions; i++){
        for(int j = 0; j <= divisions; j++){

            

            // --- Bottom face ---
            pushback_vertex(vertices, x, y_b, z);

            // --- Top face ---
            pushback_vertex(vertices, x, y_t, z);

            // --- Left face ---
            pushback_vertex(vertices, x, z, y_t);

            // --- Right face ---
            pushback_vertex(vertices, x, z, y_b);

            // --- Front face ---
            pushback_vertex(vertices, y_t, x, z);

            // --- Back face ---
            pushback_vertex(vertices, y_b, x, z);

            x += box_side_size;
        }

        z += box_side_size;
        x = xi;

    }   

    for (int i = 1; i <= divisions; i ++){
        for (int j = 1; j <= divisions; j++){

            for(int f = 0; f <= 6; f++){

                int offset = f * face_vertices;
                int bottom_left = offset + (i - 1) * vertices_per_line + (j - 1); // Bottom-left vertex index
                int bottom_right = offset + i * vertices_per_line + (j - 1);      // Bottom-right vertex index
                int top_right = offset + i * vertices_per_line + j;               // Top-right vertex index
                int top_left = offset + (i - 1) * vertices_per_line + j;          // Top-left vertex index
                if (f < 3){

                    indexes.push_back(offset + (i - 1 ) * vertices_per_line + (j - 1)); // Bottom-left vertex index
                    indexes.push_back(offset + i * vertices_per_line + (j - 1));        // Bottom-right vertex index
                    indexes.push_back(offset + i * vertices_per_line + j);              // Top-right vertex index

                    indexes.push_back(offset + (i - 1) * vertices_per_line + (j - 1)); // Bottom-left vertex index
                    indexes.push_back(offset + (i - 1) * vertices_per_line + j);       // Top-left vertex index
                    indexes.push_back(offset + i * vertices_per_line + j);             // Bottom-right vertex index

                }
                else {
                    indexes.push_back(offset + (i - 1) * vertices_per_line + (j - 1)); // Bottom-left vertex index
                    indexes.push_back(offset + i * vertices_per_line + (j - 1));       // Bottom-right vertex index
                    indexes.push_back(offset + i * vertices_per_line + j);             // Top-right vertex index

                    indexes.push_back(offset + (i - 1) * vertices_per_line + (j - 1)); // Bottom-left vertex index
                    indexes.push_back(offset + i * vertices_per_line + j);             // Bottom-right vertex index
                    indexes.push_back(offset + (i - 1) * vertices_per_line + j);       // Top-left vertex index
                }
            }

        }
    }

}