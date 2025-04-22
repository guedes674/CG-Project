#include <iostream>
#include <cstring> // For string comparison functions
#include "plane.h"
#include "box.h"
#include "sphere.h"
#include "cone.h"
#include "cylinder.h"
#include "torus.h"
#include "model.h"
#include "bezier.h" // Bezier curve generation
#include "../aux/aux.h" // Utilities for parsing and model handling

/**
 * @brief Main application entry point for the shape generator tool.
 *
 * This program generates 3D geometric shapes based on command line arguments
 * and saves them to files in a custom format that can be loaded by the rendering engine.
 *
 * @param argc  Number of command line arguments.
 * @param argv  Array of command line argument strings.
 * @return      0 on success, 1 on error.
 */
int main(int argc, char** argv){

    // Storage for the generated shape data
    vector<float> vertices;      // Holds all vertex (x,y,z) coordinates
    vector<unsigned int> indexes; // Holds all triangle indexes

    string file_name;           // Output file path
    
    // Process command arguments based on their count
    switch(argc) {
        case 2:
            // Process help command with no additional arguments
            if (strcmp(argv[1],"help") == 0){
                // Display available commands and their usage
                cout << "Available commands:\n\n";
                cout << "generator cone <base_radius> <height> <slices> <stacks> <output_file>\n";
                cout << "generator cone <base_radius> <height> <slices> <stacks> <output_file>\n";
                cout << "generator sphere <radius> <slices> <stacks> <output_file>\n";
                cout << "generator patch <patch_file> <tesselation> <output_file>\n";
                cout << "generator plane <length> <divisions> <output_file>\n";
                cout << "generator box <length> <divisions> <output_file>\n";
            }
            else {
                // Display error for invalid single argument
                cout << "Invalid arguments!\nUse command \"EngineExe help\"\n";
                return 1;
            }
            break;
        case 5:
            // Handle plane and box generation (4 parameters + command)
            if (strcmp(argv[1],"plane")==0){
                // Extract plane parameters from command line
                float length = atof(argv[2]);      // Side length
                int divisions = atoi(argv[3]);     // Subdivisions
                file_name = argv[4];               // Output file
                
                // Generate plane geometry
                plane(length, divisions, vertices, indexes);
                cout << "Plane generated with length: " << length << " and divisions: " << divisions << std::endl;
            }
            else if (strcmp(argv[1],"box")==0){
                // Extract box parameters from command line
                float length = atof(argv[2]);      // Side length
                int divisions = atoi(argv[3]);     // Subdivisions
                file_name = argv[4];               // Output file
                
                // Generate box geometry
                box(length, divisions, vertices, indexes);
                cout << "Box generated with length: " << length << " and divisions: " << divisions << std::endl;
            }
            else if (strcmp(argv[1],"patch")==0){
                // Extract bezier parameters from command line
                char* patch = argv[2];              // Patch file
                int tessellation = atoi(argv[3]);   // Tessellation level
                file_name = argv[4];                // Output file
                
                // Generate bezier curve geometry
                bezier(patch, tessellation, vertices, indexes);
                cout << "Bezier curve generated with patch: " << patch << " and tessellation: " << tessellation << std::endl;
            }
            else {
                // Display error for invalid command with 5 args
                std::cerr << "Invalid command. Use 'generator help' for usage instructions." << std::endl;
                return 1;
            }
            break;
        case 6:
            // Handle sphere generation (5 parameters + command)
            if (strcmp(argv[1],"sphere")==0){
                // Extract sphere parameters from command line
                float radius = atof(argv[2]);      // Sphere radius
                int slices = atoi(argv[3]);        // Longitude divisions
                int stacks = atoi(argv[4]);        // Latitude divisions
                file_name = argv[5];               // Output file
                
                // Generate sphere geometry
                sphere(radius, slices, stacks, vertices, indexes);
                cout << "Sphere generated with radius: " << radius << ", slices: " << slices << ", and stacks: " << stacks << std::endl;
            }
            else {
                // Display error for invalid command with 6 args
                std::cerr << "Invalid command. Use 'generator help' for usage instructions." << std::endl;
                return 1;
            }
            break;
        case 7:
            // Handle cone generation (6 parameters + command)
            if (strcmp(argv[1],"cone")==0){
                // Extract cone parameters from command line
                float base = atof(argv[2]);        // Base radius
                float height = atof(argv[3]);      // Cone height
                int slices = atoi(argv[4]);        // Circular base divisions
                int stacks = atoi(argv[5]);        // Vertical divisions
                file_name = argv[6];               // Output file
                
                // Generate cone geometry
                cone(base, height, slices, stacks, vertices, indexes);
                cout << "Cone generated with base: " << base << ", height: " << height << ", slices: " << slices << ", and stacks: " << stacks << std::endl;
            }
            else if (strcmp(argv[1],"cylinder")==0){
                // Extract cone parameters from command line
                float base = atof(argv[2]);        // Base radius
                float height = atof(argv[3]);      // Cone height
                int slices = atoi(argv[4]);        // Circular base divisions
                int stacks = atoi(argv[5]);        // Vertical divisions
                file_name = argv[6];               // Output file

                // Generate cone geometry
                cylinder(base, height, slices, stacks, vertices, indexes);
                cout << "Cylinder generated with base: " << base << ", height: " << height << ", slices: " << slices << ", and stacks: " << stacks << std::endl;
            }
            else if(strcmp(argv[1], "torus") == 0 && atoi(argv[5]) >= 1 && atoi(argv[4]) >= 1){//TORUS GENERATOR
                // Extract torus parameters from command line
                float inner_radius = atof(argv[2]); // Inner radius
                float outer_radius = atof(argv[3]); // Outer radius
                int slices = atoi(argv[4]);         // Circular base divisions
                int stacks = atoi(argv[5]);         // Vertical divisions
                file_name = argv[6];
                // Generate torus geometry
                torus(inner_radius, outer_radius, slices, stacks, vertices, indexes);
                cout << "Torus created\n";
            }
            else {
                // Display error for invalid command with 7 args
                std::cerr << "Invalid command. Use 'generator help' for usage instructions." << std::endl;
                return 1;
            }
            break;
        default:
            // Handle any other number of arguments as invalid
            std::cerr << "Invalid number of arguments. Use 'generator help' for usage instructions." << std::endl;
            return 1;
    }

    // Save the generated model data to the specified file (if not in help mode)
    if (argc > 2) {
        generate_model(file_name, vertices, indexes);
    }
    return 0;
}