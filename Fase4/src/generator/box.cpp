#include "box.h"

using namespace std;

/**
 * @brief Helper function to place vertex data at specific indexes in the vertices vector.
 * 
 * @param vertices  The vector storing all vertex coordinates.
 * @param index     Starting index where to place the vertex coordinates.
 * @param x         X-coordinate of the vertex.
 * @param y         Y-coordinate of the vertex.
 * @param z         Z-coordinate of the vertex.
 */
void pushback_vertex(vector<float>& vertices, int index, float x, float y, float z){
    vertices[index] = x;
    vertices[index + 1] = y;
    vertices[index + 2] = z;
}

/**
 * @brief Generates the data for a 3D box mesh centered at the origin.
 *
 * @param length     The total length of each box side.
 * @param divisions  Number of subdivisions per face (results in divisions x divisions quads per face).
 * @param vertices   Output vector for storing vertex positions (x, y, z).
 * @param indexes    Output vector for storing triangle indexes.
 */
void box(float length, int divisions, vector<float>& vertices, vector<unsigned int>& indexes, vector<float>& normals, vector<float>& textures) {
    // Calculate number of vertices per face
    int vertices_per_line = divisions + 1;
    int face_vertices = (vertices_per_line) * (vertices_per_line);

    // Pre-allocate memory for all vertices (6 faces × vertices_per_line² vertices × 3 coordinates)
    vertices.resize(6 * face_vertices * 3);
    normals.resize(6 * face_vertices * 3);
    textures.resize(6 * face_vertices * 2); // Texture coordinates (u, v)

    // Calculate box dimensions
    float half_length = length / 2;
    float x = -half_length;  // Start at left side
    float xi = -half_length; // Initial x value (saved for row resets)
    float z = -half_length;  // Start at back side

    // Calculate size of each grid cell
    float box_side_size = length / divisions;
    float texture_step = 1.0f / divisions; // Texture coordinate step

    // Y coordinates for top and bottom faces
    float y_t = box_side_size * divisions - half_length;   // Top face y-coordinate
    float y_b = -half_length;     // Bottom face y-coordinate

    // Generate vertices for all six faces
    for(int i = 0; i <= divisions; i++){
        for(int j = 0; j <= divisions; j++){
            // --- Bottom face (Y-) ---
            int index = (i * (vertices_per_line) + j) * 3;
            pushback_vertex(vertices, index, x, y_b, z);
            pushback_vertex(normals,index, 0.0f, -1.0f, 0.0f);
            int texture_index = (i * (divisions + 1) + j);
            textures[texture_index * 2] = float(j) * texture_step;
            textures[texture_index * 2 + 1] = float(i) * texture_step;

            // --- Front face (Z+) ---
            index = (face_vertices + i * (vertices_per_line) + j) * 3;
            pushback_vertex(vertices, index, y_t, x, z);
            pushback_vertex(normals,index, 1.0f, 0.0f, 0.0f);
            textures[(face_vertices + texture_index)* 2] = float(i) * texture_step;
            textures[(face_vertices + texture_index)* 2 + 1] = float(j) * texture_step;

            // --- Left face (X-) ---
            index = (2 * face_vertices + i * (vertices_per_line) + j) * 3;
            pushback_vertex(vertices, index, x, z, y_t);
            pushback_vertex(normals,index, 0.0f, 0.0f, 1.0f);
            textures[(2 * face_vertices + texture_index)* 2] = float(j) * texture_step;
            textures[(2 * face_vertices + texture_index)* 2 + 1] = float(i) * texture_step;

            // --- Top face (Y+) ---
            index = (3 * face_vertices + i * (vertices_per_line) + j) * 3;
            pushback_vertex(vertices, index, x, y_t, z);
            pushback_vertex(normals,index, 0.0f, 1.0f, 0.0f);
            textures[(3 * face_vertices + texture_index)* 2] = float(j) * texture_step;
            textures[(3 * face_vertices + texture_index)* 2 + 1] = float(i) * texture_step;

            // --- Back face (Z-) ---
            index = (4 * face_vertices + i * (vertices_per_line) + j) * 3;
            pushback_vertex(vertices,index, y_b, x, z);
            pushback_vertex(normals,index, -1.0f, 0.0f, 0.0f);
            textures[(4 * face_vertices + texture_index)* 2] = float(i) * texture_step;
            textures[(4 * face_vertices + texture_index)* 2 + 1] = float(j) * texture_step;

            // --- Right face (X+) ---
            index = (5 * face_vertices + i * (vertices_per_line) + j) * 3;
            pushback_vertex(vertices, index, x, z, y_b);
            pushback_vertex(normals,index, 0.0f, 0.0f, -1.0f);
            textures[(5 * face_vertices + texture_index)* 2] = float(j) * texture_step;
            textures[(5 * face_vertices + texture_index)* 2 + 1] = float(i) * texture_step;

            // Move to next horizontal position
            x += box_side_size;
        }

        // Move to next row, reset x to starting position
        z += box_side_size;
        x = xi;
    }   

    // Generate indexes for all faces
    for (int i = 1; i <= divisions; i++){
        for (int j = 1; j <= divisions; j++){

            // Calculate corner indexes for current grid cell
            float bottom_left = (i - 1) * (vertices_per_line) + (j - 1); // Bottom-left vertex index
            float bottom_right = i * (vertices_per_line) + (j - 1);      // Bottom-right vertex index
            float top_right = i * (vertices_per_line) + j;               // Top-right vertex index
            float top_left = (i - 1) * (vertices_per_line) + j;          // Top-left vertex index

            // Add triangles for bottom, front, and left faces
            // These faces share the same vertex winding order
            for(int offset = 0; offset <= 2 * face_vertices; offset += face_vertices) {
                // --- First Triangle ---
                indexes.push_back(offset + bottom_left);  // Bottom-left
                indexes.push_back(offset + top_right);    // Top-right
                indexes.push_back(offset + bottom_right); // Bottom-right

                // --- Second Triangle ---
                indexes.push_back(offset + bottom_left); // Bottom-left
                indexes.push_back(offset + top_left);    // Top-left
                indexes.push_back(offset + top_right);   // Top-right
            }

            // Add triangles for top, back, and right faces
            // These faces share a different vertex winding order to maintain proper face orientation
            for(int offset = 3 * face_vertices; offset <= 5 * face_vertices; offset += face_vertices) {
                // --- First Triangle ---
                indexes.push_back(offset + bottom_left);  // Bottom-left
                indexes.push_back(offset + bottom_right); // Bottom-right
                indexes.push_back(offset + top_right);    // Top-right

                // --- Second Triangle ---
                indexes.push_back(offset + bottom_left); // Bottom-left
                indexes.push_back(offset + top_right);   // Top-right
                indexes.push_back(offset + top_left);    // Top-left
            }
        }
    }
}