#include "cone.h"

using namespace std;

/**
 * @brief Generates the data for a 3D cone mesh centered at the origin.
 *
 * @param base       The radius of the cone base.
 * @param height     The height of the cone from base to tip.
 * @param slices     Number of subdivisions around the circular base.
 * @param stacks     Number of subdivisions along the height of the cone.
 * @param vertices   Output vector for storing vertex positions (x, y, z).
 * @param indexes    Output vector for storing triangle indices.
 */
void cone(float base, float height, int slices, int stacks, vector<float>& vertices, vector<unsigned int>& indexes) {
    // Calculate angular increment for base circle
    float base_angle_increment = 2 * M_PI / slices;
    
    // Calculate height and radius increments for stacks
    float height_increment = height / stacks;
    float radius_decrement = base / stacks;
    
    // Initialize tracking variables
    float current_angle, current_height, current_radius;
    current_height = 0;
    current_radius = base;
    current_angle = 0;

    // Generate base circle vertices
    for(int i = 0; i < slices; i++){
        current_angle = base_angle_increment * i;
        vertices.push_back(current_radius * sin(current_angle)); // x coordinate
        vertices.push_back(0);                                  // y coordinate (base)
        vertices.push_back(current_radius * cos(current_angle)); // z coordinate
    }

    // Generate vertices for cone body (stack rings)
    for(int i = 0; i < stacks; i++){
        current_height = height_increment * i;
        current_radius = base - radius_decrement * i;
current_angle = 0;

        for(int j = 0; j < slices; j++){
            current_angle = base_angle_increment * j;
            
            vertices.push_back(current_radius * sin(current_angle)); // x coordinate
            vertices.push_back(current_height);                     // y coordinate
            vertices.push_back(current_radius * cos(current_angle)); // z coordinate
        }
    }

    // Add base center vertex
    vertices.push_back(0); // x coordinate
    vertices.push_back(0); // y coordinate
    vertices.push_back(0); // z coordinate

    // Add cone tip vertices (one for each slice segment)
    for(int i = 0; i < slices; i++){
        vertices.push_back(0);      // x coordinate
        vertices.push_back(height); // y coordinate
        vertices.push_back(0);      // z coordinate
    }

    // Calculate total vertex count for index generation
    unsigned int total_vertices = vertices.size() / 3;

    // Generate indices for cone body (connecting stack rings)
    for (int i = 1; i <= stacks; i++){
        for(int j = 1; j < slices; j++){
            // First triangle (connecting vertices between adjacent stacks)
            indexes.push_back(i * slices + (j -1));      // Bottom-left vertex
            indexes.push_back((i - 1) * slices + j);     // Bottom-right vertex
            indexes.push_back(i * slices + j);           // Top-right vertex

            // Second triangle (completing the quad)
            indexes.push_back((i - 1) * slices + (j - 1)); // Bottom-left vertex
            indexes.push_back((i - 1) * slices + j);       // Bottom-right vertex
            indexes.push_back(i * slices + (j - 1));       // Top-left vertex
        } 

        // Connect last segment in the ring (closing the loop)
        indexes.push_back((i - 1) * slices);               // First vertex of previous stack
        indexes.push_back(i * slices);                     // First vertex of current stack
        indexes.push_back(i * slices + (slices - 1));      // Last vertex of current stack

        indexes.push_back((i - 1) * slices + (slices - 1)); // Last vertex of previous stack
        indexes.push_back((i - 1) * slices);                // First vertex of previous stack
        indexes.push_back(i * slices + (slices - 1));       // Last vertex of current stack
    }

    // Generate triangles for the base (connecting base center to base circle)
    for (int j = 1; j < slices; j++) {
        indexes.push_back(total_vertices - 1 - slices);    // Base center
        indexes.push_back(j);                              // Current vertex on base
        indexes.push_back(j - 1);                          // Previous vertex on base
    }

    // Close the base circle
    indexes.push_back(total_vertices - 1 - slices);        // Base center
    indexes.push_back(0);                                  // First vertex on base
    indexes.push_back(slices - 1);                         // Last vertex on base

    // Generate triangles for the top (connecting tip vertices to top ring)
    for (int j = 1; j < slices; j++) {
        indexes.push_back((stacks) * (slices) + (j - 1));  // Previous vertex on top ring
        indexes.push_back((stacks) * (slices) + j);        // Current vertex on top ring
        indexes.push_back(total_vertices - slices + j - 1); // Corresponding tip vertex
    }

    // Close the top circle
    indexes.push_back((stacks) * (slices) + (slices - 1)); // Last vertex on top ring
    indexes.push_back((stacks) * (slices));                // First vertex on top ring
    indexes.push_back(total_vertices - 1);                 // Last tip vertex
}