#include "bezier.h"
#include "../aux/aux.h"
#include "../aux/curves.h"

using namespace std;

/**
 * @brief Reads a patch file containing control points for Bezier surfaces.
 *
 * The patch file format:
 * - First line: number of patches
 * - Next N lines: indices of control points for each patch (16 indices per patch)
 * - Next line: number of control points
 * - Next M lines: x,y,z coordinates of each control Vector3
 *
 * @param patch           Path to the patch file.
 * @param points          Output vector for storing control points.
 * @param indexes_matrix  Output vector of vectors for storing patch indices.
 * @return                0 if successful, 1 if file opening failed.
 */
int read_patch_file(char* patch, vector<Vector3> &points, vector<vector<int>> &indexes_matrix){
    std::ifstream file;
    file.open(patch, ios::in);

    if(file.fail()) return 1;
    else {
        int index_lines = 0;
        string line;
        string delimiter = ",";

        // Read number of patches
        getline(file, line);
        index_lines = stoi(line);

        // Reserve space for patch indices
        indexes_matrix.reserve(index_lines);

        // Read patch indices (16 control points per patch)
        for(int i = 0; i < index_lines; i++){
            getline(file,line);

            vector<int> index_line;

            // Parse comma-separated list of indices
            vector<string> input = parse_line(line,delimiter);
            int line_size = input.size();
            index_line.reserve(line_size);

            // Convert string indices to integers
            for(int j = 0; j < line_size; j++){
                index_line.push_back(stoi(input[j]));
            }
            indexes_matrix.push_back(index_line);
        }

        // Read number of control points
        getline(file,line);
        int points_lines = stoi(line);

        // Reserve space for control points
        points.reserve(points_lines);
        
        // Read control Vector3 coordinates
        for(int i = 0; i < points_lines; i++){
            getline(file,line);
            
            // Parse comma-separated coordinates
            vector<string> input = parse_line(line, delimiter);
            
            // Create Vector3 from x,y,z coordinates
            points.push_back(Vector3(stof(input[0]),stof(input[1]),stof(input[2])));
        }
    }
    return 0;
}

/**
 * @brief Generates a Bezier surface mesh using control points from a patch file.
 *
 * This function reads control points from a patch file and generates a 3D mesh
 * representing the Bezier surface. It calculates surface points using the Bernstein
 * polynomials and creates triangles to form the surface.
 *
 * @param patch         Path to the patch file containing control points.
 * @param tessellation  Level of tessellation (resolution) for the surface.
 * @param vertices      Output vector for storing vertex positions (x, y, z).
 * @param indexes       Output vector for storing triangle indexes.
 */
void bezier(char* patch, int tessellation, vector<float>&vertices, vector<unsigned int>& indexes, vector<float>& normals, vector<float>& textures){

    vector<Vector3> points;
    vector<vector <int>> indexes_matrix;

    // Load control points and patch indices from file
    read_patch_file(patch, points, indexes_matrix);
    int total_patches = indexes_matrix.size();

    // Reserve space for vertices (3 coordinates per vertex, tessellation+1 vertices per row/column, per patch)
    vertices.reserve(total_patches * (tessellation + 1) * (tessellation + 1) * 3);
    
    // Calculate parametric increment based on tessellation level
    float inc = 1.0f / tessellation;
    
    // Process each patch
    for (int p = 0; p < total_patches; p++) {
        int* index_patch = indexes_matrix[p].data();
        float control_points[4][4][3];  // 4x4 grid of control points (x,y,z)
    
        // Organize control points into a 4x4 grid for easier calculation
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                // Get Vector3 from the loaded control points
                Vector3 pt = points[index_patch[i * 4 + j]];
                control_points[i][j][0] = pt.x;
                control_points[i][j][1] = pt.y;
                control_points[i][j][2] = pt.z;
            }
        }
    
        // Generate surface points by evaluating the Bezier patch at regular parameter intervals
        for (int v = 0; v <= tessellation; v++) {
            float t_v = v * inc;  // Parametric v coordinate [0,1]
            for (int u = 0; u <= tessellation; u++) {
                float t_u = u * inc;  // Parametric u coordinate [0,1]
    
                // Arrays to store the calculated Vector3 and its derivatives
                float Vector3[3] = {0};  // Surface Vector3 (x,y,z)
                float du[3] = {0};       // Tangent vector in u direction
                float dv[3] = {0};       // Tangent vector in v direction        
                // Calculate surface Vector3 using bicubic Bezier formula with Bernstein polynomials
                for (int i = 0; i < 4; i++) {
                    float bi = bernstein(i, 3, t_u);       // Bernstein polynomial for u
                    float bi_deriv = bernstein_deriv(i, t_u); // Derivative of Bernstein polynomial for u
                    for (int j = 0; j < 4; j++) {
                        float bj = bernstein(j, 3, t_v);       // Bernstein polynomial for v
                        float bj_deriv = bernstein_deriv(j, t_v); // Derivative of Bernstein polynomial for v
                        // Calculate position and derivatives for x, y, and z
                        for (int k = 0; k < 3; k++) {
                            float cp = control_points[i][j][k];
                            Vector3[k] += bi * bj * cp;      // Surface Vector3
                            du[k] += bi_deriv * bj * cp;         // Tangent in u direction
                            dv[k] += bi * bj_deriv * cp;         // Tangent in v direction
                
                        }
                    }
                }
    
                // Store the calculated vertex position
                for (int k = 0; k < 3; k++) {
                    vertices.push_back(Vector3[k]);
                }

                // Calculate the normal vector using the tangents
                float normal[3];
                calculate_normal(du, dv, normal);

                // Store the calculated normal
                for (int k = 0; k < 3; k++) {
                    normals.push_back(normal[k]);
                }

                // Store the texture coordinates (u,v)
                textures.push_back(float((tessellation) - v)/float(tessellation));
                textures.push_back(float((tessellation) - u)/float(tessellation));
            }
        }
    }

    // Generate triangle indices to form the surface mesh
    indexes.reserve(total_patches * tessellation * tessellation * 6); // 2 triangles per grid cell, 3 indices per triangle
    
    // For each patch, generate triangles by connecting adjacent vertices
    for (int i = 0; i < total_patches; i++) {
        for (int y = 1; y <= tessellation; y++) {
            for (int x = 1; x <= tessellation; x++) {
                int patch_offset = i * (tessellation + 1) * (tessellation + 1);
                int index = patch_offset + y * (tessellation + 1) + x;
                int index_left = index - 1;
                int index_top = index - (tessellation + 1);
                int index_top_left = index_top - 1;

                indexes.push_back(index);
                indexes.push_back(index_top_left);
                indexes.push_back(index_left);

                indexes.push_back(index_top_left);
                indexes.push_back(index);
                indexes.push_back(index_top);
            }
        }
    }
}