#include <iostream>
#include <cstring> // Add this for strcmp
#include "plane.h"
#include "box.h"
#include "sphere.h"
#include "model.h"
#include "../aux/aux.h" // Add this to ensure aux.h is included

int main(int argc, char** argv){

    vector<float> vertices;
    vector<unsigned int> indices;

    string file_name;

    switch(argc) {
        case 2:
            if (strcmp(argv[1],"help") == 0){
                cout << "Available commands:\n\n";
                cout << "EngineExe cone <base_radius> <height> <slices> <stacks>\n";
                cout << "EngineExe sphere <radius> <slices> <stacks>\n";
                cout << "EngineExe plane <length> <divisions> \n";
                cout << "EngineExe box <length> <divisions>\n";
            }
            else {
                cout << "Invalid arguments!\nUse command \"EngineExe help\"\n";
                return 1;
            }
            break;
        case 5:
            if (strcmp(argv[1],"plane")==0){
                float length = atof(argv[2]);
                int divisions = atoi(argv[3]);
                file_name = argv[4];
                plane(length, divisions, vertices, indices);
                cout << "Plane generated with length: " << length << " and divisions: " << divisions << std::endl;
            }
            else if (strcmp(argv[1],"box")==0){
                float length = atof(argv[2]);
                int divisions = atoi(argv[3]);
                file_name = argv[4];
                box(length, divisions, vertices, indices);
                cout << "Box generated with length: " << length << " and divisions: " << divisions << std::endl;
            }
            else {
                std::cerr << "Invalid command. Use 'generator help' for usage instructions." << std::endl;
                return 1;
            }
            break;
        case 6:
            if (strcmp(argv[1],"sphere")==0){
                float radius = atof(argv[2]);
                int slices = atoi(argv[3]);
                int stacks = atoi(argv[4]);
                file_name = argv[5];
                sphere(radius, slices, stacks, vertices, indices);
                cout << "Sphere generated with radius: " << radius << ", slices: " << slices << ", and stacks: " << stacks << std::endl;
            }
            else {
                std::cerr << "Invalid command. Use 'generator help' for usage instructions." << std::endl;
                return 1;
            }
            break;
        default:
            std::cerr << "Invalid number of arguments. Use 'generator help' for usage instructions." << std::endl;
            return 1;
    }

    // Generate the model file
    generate_model(file_name, vertices, indices);
    return 0;
}