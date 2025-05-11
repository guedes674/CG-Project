#include "model.h"

using namespace std;

/**
 * @brief Saves the vertex and index data of a 3D model to a file.
 *
 * @param file_name Name of the file to write to.
 * @param vertices  Vector containing the model's vertex positions.
 * @param indexes   Vector containing the model's index data.
 * @return 0 if successful, 1 if file could not be opened.
 */
int generate_model(string file_name, vector<float>& vertices, vector<unsigned int>& indexes, vector<float>& normals, vector<float>& textures) {

    ofstream file;
    file.open(file_name, ios::out); // Open file in output mode

    if (file.is_open()) {
        // --- Write vertex positions ---
        for (size_t i = 0; i < vertices.size(); i++) {
            file << vertices[i];

            // Add delimiter except after last value
            if (i < vertices.size() - 1)
                file << ";";
        }
        file << "\n"; // New line after vertex data

        // --- Write index data ---
        for (size_t i = 0; i < indexes.size(); i++) {
            file << indexes[i];

            // Add delimiter except after last value
            if (i < indexes.size() - 1)
                file << ";";
        }
        file << "\n"; // New line after index data

        // --- Write normal data ---
        for (size_t i = 0; i < normals.size(); i++) {
            file << normals[i];

            // Add delimiter except after last value
            if (i < normals.size() - 1)
                file << ";";
        }
        file << "\n"; // New line after normal data

        // --- Write textures data ---
        for (size_t i = 0; i < textures.size(); i++) {
            file << textures[i];

            // Add delimiter except after last value
            if (i < textures.size() - 1)
                file << ";";
        }
        file << "\n"; // New line after normal data

        file.close(); // Finalize file
        return 0;     // Success
    }

    return 1; // Error opening file
}


/**
 * @brief Reads a 3D model's vertex and index data from a file.
 *
 * The file format contains two lines:
 * 1. First line: All vertex coordinates as semicolon-separated values (x1;y1;z1;x2;y2;z2;...)
 * 2. Second line: All triangle indexes as semicolon-separated values (i1;i2;i3;i4;i5;i6;...)
 *
 * @param file_name Name of the file to read from.
 * @param vertices  Vector to store the read vertex data (x, y, z).
 * @param indexes   Vector to store the read triangle indexes.
 * @return 0 if successful, 1 if file could not be opened.
 */
int read_model(string file_name, vector<float>& vertices, vector<unsigned int>& indexes,vector<float>& normals, vector<float>& textures, float* bounding_box, Vector3 center, float radius) {

    ifstream file;
    file.open(file_name, ios::in); // Open file in input mode

    float value = 0;   // Temporary storage for numeric conversion

    if (file.fail()) return 1; // Failed to open file

    else {
        // Initialize the bounding box to ±infinity
        bounding_box[0] = bounding_box[2] = bounding_box[4] =  std::numeric_limits<float>::max();   // mins
        bounding_box[1] = bounding_box[3] = bounding_box[5] =  std::numeric_limits<float>::lowest(); // maxs
        string line;             // String to hold each line read from file
        string delimiter = ";";  // Delimiter used to split values

        // --- First Lines ---
        getline(file, line);                           // Read the first line
        vector<string> v = parse_line(line, delimiter); // Split line into float strings
        vertices.reserve(v.size());                    // Reserve memory in vector for efficiency

        // Process each value and add it to the vertices vector
        for (int i = 0; static_cast<unsigned long>(i) < v.size(); i++) {
            value = stof(v[i]);           // Convert string to float

            size_t axis = i % 3;  // 0=x, 1=y, 2=z

            // update min/max for this axis
            bounding_box[axis*2 + 0] = std::min(bounding_box[axis*2 + 0], value);
            bounding_box[axis*2 + 1] = std::max(bounding_box[axis*2 + 1], value);

            vertices.push_back(value);    // Store value in vertex vector
            
        }
        float center_x = (fabs(bounding_box[0]) + fabs(bounding_box[1]))/2;
        float center_y = (fabs(bounding_box[2]) + fabs(bounding_box[3]))/2;
        float center_z = (fabs(bounding_box[4]) + fabs(bounding_box[5]))/2;
        center = Vector3(center_x,center_y,center_z);

        radius = fabs(bounding_box[1])-center_x;
        // --- Second Line ---
        getline(file, line);                            // Read second line
        vector<string> iv = parse_line(line, delimiter); // Split line into unsigned int strings
        indexes.reserve(iv.size());                     // Reserve memory in index vector

        // Process each value and add it to the indexes vector
        for (int i = 0; static_cast<unsigned long>(i) < iv.size(); i++)
            indexes.push_back(stoul(iv[i]));            // Convert string to unsigned long and store as index

        // --- Third Line ---
        getline(file, line);  
        vector<string> nv = parse_line(line,delimiter);
        normals.reserve(nv.size());
        
        for (int i = 0; static_cast<unsigned long>(i) < nv.size(); i++)
            normals.push_back(stoul(nv[i]));            // Convert string to unsigned long and store as index

        // --- Fourth Line ---
        getline(file, line);  
        vector<string> tv = parse_line(line,delimiter);
        textures.reserve(tv.size());
        
        for (int i = 0; static_cast<unsigned long>(i) < tv.size(); i++)
            textures.push_back(stoul(tv[i]));            // Convert string to unsigned long and store as index

        file.close();   // Close the file when done
        return 0;       // Success - file was read correctly
    }
}
