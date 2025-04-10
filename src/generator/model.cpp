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
int generate_model(string file_name, vector<float>& vertices, vector<unsigned int>& indexes) {

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

        file.close(); // Finalize file
        return 0;     // Success
    }

    return 1; // Error opening file
}


/**
 * @brief Reads a 3D model's vertex and index data from a file.
 *
 * @param file_name Name of the file to read from.
 * @param vertices  Vector to store the read vertex data (x, y, z).
 * @param indexes   Vector to store the read triangle indices.
 * @return 0 if successful, 1 if file could not be opened.
 */
int read_model(string file_name, vector<float>& vertices, vector<unsigned int>& indexes) {

    ifstream file;
    file.open(file_name, ios::in); // Open file in input mode

    int index = 0;     // Tracks component index (x, y, z), if needed
    float value = 0;   // Temporary storage for numeric conversion

    if (file.fail()) return 1; // Failed to open file

    else {
        string line;             // String to hold each line read from file
        string delimiter = ";";  // Delimiter used to split values

        // ==========================
        // Read vertices (first line)
        // ==========================
        getline(file, line);                           // Read the first line
        vector<string> v = parseLine(line, delimiter); // Split line into float strings
        vertices.reserve(v.size());                    // Reserve memory in vector

        for (int i = 0; static_cast<unsigned long>(i) < v.size(); i++) {
            value = stof(v[i]);           // Convert string to float
            vertices.push_back(value);    // Store value in vertex vector
            index = (index + 1) % 3;      // Update index cycle (optional use)
        }

        // ==========================
        // Read indices (second line)
        // ==========================
        getline(file, line);                            // Read second line
        vector<string> iv = parseLine(line, delimiter); // Split line into unsigned ints
        indexes.reserve(iv.size());                     // Reserve memory in index vector

        for (int i = 0; static_cast<unsigned long>(i) < iv.size(); i++)
            indexes.push_back(stoul(iv[i]));            // Convert and store index

        return 0; // Success
    }
}
