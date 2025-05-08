#include "plane.h"
using namespace std;

/**
 * @brief Generates the data for a subdivided plane mesh centered at the origin on the XZ plane.
 *
 * @param length     The total length of the plane's side.
 * @param divisions  Number of subdivisions per side (results in divisions x divisions quads).
 * @param vertices    Output vector for storing vertex positions (x, y, z).
 * @param indexes    Output vector for storing triangle indexes.
 */
void plane(float length, int divisions, vector<float>& vertices, vector<unsigned int>& indexes, vector<float>& normals, vector<float>& textures) {

    // Inicial x value (leftmost point)
    float xi = -length/2;
    // Current x (will iterate horizontally)
    float x = xi;
    // Y value is constant (flat plane)
    float y = 0;
    // initial z coordinate (bottom point)
    float z = -length/2;

    // Number of boxes (subdivisions)
    float box_side_size = length / divisions;

    // Generating the vertices of the plane
    for (int i = 0; i <= divisions; i++) {

        for (int j = 0; j <= divisions; j++) {
            // --- Vertex positions ---
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            
            // --- Normal vector ---
            normals.push_back(0.0f); // Normal x (pointing up)
            normals.push_back(1.0f); // Normal y (pointing up)
            normals.push_back(0.0f); // Normal z (pointing up)

            // --- Texture coordinates ---
            textures.push_back(float(i) / divisions); // Texture coordinate u
            textures.push_back(float(j) / divisions); // Texture coordinate v
            
            // Move to next horizontal position
            x += box_side_size;
        }

        // Move to next row (forward along z), reset x to start
        x = xi;
        z += box_side_size;
    }

    // Generating the indexes of the plane
    for (int i=1; i <= divisions; i++) {

        for (int j=1; j <= divisions; j++) {

            // --- Triangle indexes ---
            float bottom_left = (i - 1) * (divisions + 1) + (j - 1); // Bottom-left vertex index
            float bottom_right = i * (divisions + 1) + (j - 1);      // Bottom-right vertex index
            float top_right = i * (divisions + 1) + j;               // Top-right vertex index
            float top_left = (i - 1) * (divisions + 1) + j;          // Top-left vertex index

            // --- First Triangle ---
            indexes.push_back(bottom_left);  // Bottom-left
            indexes.push_back(bottom_right); // Bottom-right
            indexes.push_back(top_right);    // Top-right

            // --- Second Triangle ---
            indexes.push_back(bottom_left); // Bottom-left
            indexes.push_back(top_right);   // Top-right
            indexes.push_back(top_left);    // Top-left
        }
    }

}
