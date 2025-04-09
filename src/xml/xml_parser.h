#ifndef XML_PARSER_H
#define XML_PARSER_H

#include "tinyxml2.h"
#include <iostream>
#include <vector>

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
        float far,near,fov; // far, near and fov
};

class modelxml {
    public: 
        char * file_name;
};

class groupxml {
    public:
        std::vector<modelxml> models;
};

class xmlparser {
    public:
        window win;
        camera cam;
        std::vector<groupxml> groups;
        int get_int_attribute(XMLElement* element, const char* attributeName, int defaultValue);
};

#endif // XML_PARSER_H