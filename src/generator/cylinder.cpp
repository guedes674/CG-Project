#include "cylinder.h"

#include <vector>

/**
 * @brief Generates the data for a 3D cylinder mesh centered at the origin.
 *
 * @param radius     The radius of the cylinder.
 * @param height     The height of the cylinder.
 * @param slices     Number of subdivisions around the circular base/top.
 * @param stacks     Number of subdivisions along the height of the cylinder.
 * @param vertices   Output vector for storing vertex positions (x, y, z).
 * @param indexes    Output vector for storing triangle indexes.
 */
void cylinder(float radius, float height, int slices, int stacks, std::vector<float>& vertices, std::vector<unsigned int>& indexes, std::vector<float>& normals, std::vector<float>& textures) {
    // Calculate angular increment for circle
    float angle_increment = 2 * M_PI / slices;
    
    // Calculate height increment for stacks
    float height_increment = height / stacks;
    
    // Initialize tracking variables
    float current_angle, current_height;
    current_angle = 0;

    // Generate base circle vertices
    for (int i = 0; i < slices; i++) {
        current_angle = angle_increment * i;
        float x = radius * sin(current_angle);
        float z = radius * cos(current_angle);

        vertices.push_back(x); // x coordinate
        vertices.push_back(0); // y coordinate (base)
        vertices.push_back(z); // z coordinate

        normals.push_back(0.0f);  // Normal x (pointing outwards)
        normals.push_back(-1.0f); // Normal y (pointing downwards)
        normals.push_back(0.0f);  // Normal z (pointing outwards)

        textures.push_back(float(i) / slices); // Texture coordinate u
        textures.push_back(0.0f);              // Texture coordinate v (base)
    }

    // Generate vertices and normals for cylinder body (stack rings)
    for (int i = 0; i <= stacks; i++) {
        current_height = height_increment * i;

        for (int j = 0; j < slices; j++) {
            current_angle = angle_increment * j;
            float x = radius * sin(current_angle);
            float z = radius * cos(current_angle);

            vertices.push_back(x); // x coordinate
            vertices.push_back(current_height); // y coordinate
            vertices.push_back(z); // z coordinate

            normals.push_back(x / radius);  // Normal x (pointing outward)
            normals.push_back(0.0f);        // Normal y (no vertical component)
            normals.push_back(z / radius);  // Normal z (pointing outward)

            textures.push_back(float(j) / slices); // Texture coordinate u
            textures.push_back(float(i) / stacks); // Texture coordinate v
        }
    }

    // Add base center vertex
    vertices.push_back(0); // x coordinate
    vertices.push_back(0); // y coordinate (bottom)
    vertices.push_back(0); // z coordinate

    normals.push_back(0.0f);  // Normal x
    normals.push_back(-1.0f); // Normal y (pointing downwards)
    normals.push_back(0.0f);  // Normal z

    textures.push_back(0.5f); // Texture coordinate u
    textures.push_back(0.5f); // Texture coordinate v

    // Add top center vertex
    vertices.push_back(0); // x coordinate
    vertices.push_back(height); // y coordinate (top)
    vertices.push_back(0); // z coordinate

    normals.push_back(0.0f);  // Normal x
    normals.push_back(1.0f);  // Normal y (pointing upwards)
    normals.push_back(0.0f);  // Normal z

    textures.push_back(0.5f); // Texture coordinate u
    textures.push_back(0.5f); // Texture coordinate v

    // Calculate total vertex count and indices
    unsigned int base_center_index = (stacks + 1) * slices + slices;
    unsigned int top_center_index = base_center_index + 1;
    unsigned int top_circle_start = (stacks) * slices + slices;

    // Generate indexes for cylinder body (connecting stack rings)
    for (int i = 1; i <= stacks; i++) {
        for (int j = 1; j < slices; j++) {
            // First triangle (connecting vertices between adjacent stacks)
            indexes.push_back(i * slices + (j - 1) + slices);      // Bottom-left vertex
            indexes.push_back((i - 1) * slices + j + slices);     // Bottom-right vertex
            indexes.push_back(i * slices + j + slices);           // Top-right vertex

            // Second triangle (completing the quad)
            indexes.push_back((i - 1) * slices + (j - 1) + slices); // Bottom-left vertex
            indexes.push_back((i - 1) * slices + j + slices);       // Bottom-right vertex
            indexes.push_back(i * slices + (j - 1) + slices);       // Top-left vertex
        }

        // Connect last segment in the ring (closing the loop)
        indexes.push_back((i - 1) * slices + slices);               // First vertex of previous stack
        indexes.push_back(i * slices + slices);                     // First vertex of current stack
        indexes.push_back(i * slices + (slices - 1) + slices);      // Last vertex of current stack

        indexes.push_back((i - 1) * slices + (slices - 1) + slices); // Last vertex of previous stack
        indexes.push_back((i - 1) * slices + slices);                // First vertex of previous stack
        indexes.push_back(i * slices + (slices - 1) + slices);       // Last vertex of current stack
    }

    // Generate triangles for the base (connecting base center to base circle)
    for (int j = 1; j < slices; j++) {
        indexes.push_back(base_center_index); // Base center
        indexes.push_back(j); // Current vertex on base
        indexes.push_back(j - 1); // Previous vertex on base
    }

    // Close the base circle
    indexes.push_back(base_center_index); // Base center
    indexes.push_back(0); // First vertex on base
    indexes.push_back(slices - 1); // Last vertex on base

    // Generate triangles for the top (connecting center to top circle)
    for (int j = 1; j < slices; j++) {
        indexes.push_back(top_center_index); // Top center
        indexes.push_back(top_circle_start + j - 1); // Previous vertex on top
        indexes.push_back(top_circle_start + j); // Current vertex on top
    }

    // Close the top circle
    indexes.push_back(top_center_index); // Top center
    indexes.push_back(top_circle_start + slices - 1); // Last vertex on top
    indexes.push_back(top_circle_start); // First vertex on top
}