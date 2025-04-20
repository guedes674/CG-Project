#ifndef XML_PARSER_H
#define XML_PARSER_H

#include "tinyxml2.h"
#include <iostream>
#include <vector>
#include <string>
#include "../aux/point.h"

using namespace std;
using namespace tinyxml2;

extern int global_id; 

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

class rotation_xml {
    public:
        int time;
        float angle;
        float x, y, z;
        int order;
};

class time_transformation_xml {
    public:
        int align = 0;
        vector<Vector3> points;
        int tracking = 0; // Removed reference
        int tracking_id;
        
        // Add default constructor
        time_transformation_xml() = default;
        
        time_transformation_xml(int align,int tracking_flag ,std::vector<Vector3> points) {
            this->align = align;
            this->tracking = tracking_flag;
            this->tracking_id = global_id++;
            this->points = points;
        }
};

class translation_xml {
    public:
        float x, y, z;
        int order;
        int time;
        time_transformation_xml time_trans;
};

class scale_xml {
    public:
        float x, y, z;
        int order;
};

class transformations_xml {
    public:
        rotation_xml rotation;
        translation_xml translation;
        scale_xml scale;
        int rotation_exists = 0,translation_exists = 0,scale_exists = 0;
        int order;
};

class group_xml {
    public:
        std::vector<model_xml> models; // Removed reference
        transformations_xml transformations; // Removed reference
        std::vector<group_xml> groups; // Removed reference
};

class xml_parser {
    public:
        window win;
        camera cam;
        std::vector<group_xml> groups; // Removed reference
};

// Function prototypes
float get_float_attribute(XMLElement* element, const char* attribute_name, float default_value);
int get_int_attribute(XMLElement* element, const char* attribute_name, int default_value);
string get_string_attribute(XMLElement* element, const char* attribute_name, string default_value); // Return string instead of char*
group_xml recursive_catch_groups(XMLElement* group);
xml_parser read_xml_file(string file_name);

#endif // XML_PARSER_H