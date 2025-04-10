#ifndef XML_PARSER_H
#define XML_PARSER_H

#include "tinyxml2.h"
#include <iostream>
#include <vector>
#include <string>

using namespace std;
using namespace tinyxml2;

class window {
    public:
        int width;
        int height;
};

class camera {
    public:
        float px, py, pz; // position
        float lx, ly, lz; // look at
        float ux, uy, uz; // up vector
        float far, near, fov; // far, near and fov
};

class model_xml {
    public: 
        string file_name; // Changed from char* to std::string
};

class group_xml {
    public:
        std::vector<model_xml> models; // Removed reference
};

class xml_parser {
    public:
        window win;
        camera cam;
        std::vector<group_xml> groups; // Removed reference
};

// Function prototypes
float get_float_attribute(XMLElement* element, const char* attributeName, float defaultValue);
int get_int_attribute(XMLElement* element, const char* attributeName, int defaultValue);
string get_string_attribute(XMLElement* element, const char* attributeName, string defaultValue); // Return string instead of char*
xml_parser read_xml_file(string file_name);

#endif // XML_PARSER_H