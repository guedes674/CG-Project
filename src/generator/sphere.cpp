#include "sphere.h"

using namespace std;

/**
 * @brief Generates the data for a 3D sphere mesh centered at the origin.
 *
 * @param radius     The radius of the sphere.
 * @param slices     Number of subdivisions around the vertical axis (longitude).
 * @param stacks     Number of subdivisions along the vertical axis (latitude).
 * @param vertices   Output vector for storing vertex positions (x, y, z).
 * @param indexes    Output vector for storing triangle indexes.
 */
void sphere(float radius, int slices, int stacks, vector<float>& vertices, vector<unsigned int>& indexes){

    // Calculate total number of vertices including poles
    unsigned int total_vertices = (slices + 1) * (stacks + 1) + 2; // +2 for poles
    vertices.resize(total_vertices * 3);

    // Define the bottom pole (south pole) at (0, -radius, 0)
    int lowest_pole_index = ((int) total_vertices - 2) * 3; // Index of the lowest pole
    vertices[lowest_pole_index] = 0;
    vertices[lowest_pole_index + 1] = -radius;
    vertices[lowest_pole_index + 2] = 0;

    // Define the top pole (north pole) at (0, radius, 0)
    int highest_pole_index = ((int) total_vertices - 1) * 3; // Index of the highest pole
    vertices[highest_pole_index] = 0;
    vertices[highest_pole_index + 1] = radius;
    vertices[highest_pole_index + 2] = 0;

    // Calculate angular increments for latitude and longitude
    float horizontal_increment = M_PI / stacks;     // Increment for latitude (vertical angle)
    float vertical_increment = 2 * M_PI / slices;   // Increment for longitude (horizontal angle)

    float angle, height, horizontal_radius;

    // Generate vertices for all latitudes (except poles)
    for(int i = 0; i < stacks - 1; i++){
        // Calculate vertical position and circle radius at this latitude
        height = radius * sin(-M_PI_2 + horizontal_increment * (float)(i + 1));
        horizontal_radius = radius * cos(-M_PI_2 + horizontal_increment * (float) (i + 1));

        // Generate vertices around this latitude circle
        for(int j = 0; j <= slices; j++){
            // Calculate angle for current longitude position
            angle = vertical_increment * j;
            int index = (i * (slices + 1) + j) * 3;

            // Compute vertex position using spherical coordinates
            vertices[index] = horizontal_radius * sin(angle);       // x coordinate
            vertices[index + 1] = height;                           // y coordinate 
            vertices[index + 2] = horizontal_radius * cos(angle);   // z coordinate
        }
    }

    // Generate triangles connecting poles to first and last latitude rings
    for(int i = 0; i < slices; i++){
        // Bottom pole triangles (connecting south pole to first latitude)
        indexes.push_back(i);                       // Vertex on first latitude
        indexes.push_back(total_vertices - 2);      // South pole
        indexes.push_back(i + 1);                   // Next vertex on first latitude
        
        // Top pole triangles (connecting north pole to last latitude)
        indexes.push_back(total_vertices - 1);      // North pole
        indexes.push_back((stacks - 2) * (slices + 1) + i);       // Vertex on last latitude
        indexes.push_back((stacks - 2) * (slices + 1) + i + 1);   // Next vertex on last latitude
    }

    // Close the gap at longitude seam (where longitude = 0 and longitude = 2π meet)
    // South pole closing triangle
    indexes.push_back(slices);                      // Last vertex in first latitude
    indexes.push_back(total_vertices - 2);          // South pole
    indexes.push_back(0);                           // First vertex in first latitude (wrapping around)

    // North pole closing triangle
    indexes.push_back(total_vertices - 1);          // North pole
    indexes.push_back((stacks - 2) * (slices + 1) + slices);   // Last vertex in last latitude
    indexes.push_back((stacks - 2) * (slices + 1));            // First vertex in last latitude

    // Generate triangles for the rest of the sphere (between latitude rings)
    for(int i = 0; i < stacks - 2; i++){
        for(int j = 0; j < slices; j++){
            // Current quad corners
            unsigned int current = i * (slices + 1) + j;           // Current vertex
            unsigned int next_horiz = i * (slices + 1) + j + 1;    // Next vertex horizontally
            unsigned int next_vert = (i + 1) * (slices + 1) + j;   // Next vertex vertically
            unsigned int next_diag = (i + 1) * (slices + 1) + j + 1; // Diagonal vertex

            // First triangle - shape: '|\'
            indexes.push_back(current);
            indexes.push_back(next_horiz);
            indexes.push_back(next_vert);

            // Second triangle - shape: '\|'
            indexes.push_back(next_horiz);
            indexes.push_back(next_diag);
            indexes.push_back(next_vert);
        }
    }
}