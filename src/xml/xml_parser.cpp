#include "xml_parser.h"

float get_float_attribute(XMLElement* element, const char* attributeName, float defaultValue) {
    const char* value = element->Attribute(attributeName);
    return value ? stof(value) : defaultValue;
}

int get_int_attribute(XMLElement* element, const char* attributeName, int defaultValue) {
    const char* value = element->Attribute(attributeName);
    return value ? stoi(value) : defaultValue;
}

string get_string_attribute(XMLElement* element, const char* attributeName, string defaultValue) {
    const char* value = element->Attribute(attributeName);
    return value ? string(value) : defaultValue;
}

xml_parser read_xml_file(string file_name){
    xml_parser parser;
    XMLDocument doc;
    XMLError eResult = doc.LoadFile(file_name.c_str());

    if (eResult != XML_SUCCESS) {
        cout << "Erro ao carregar arquivo XML: " << doc.ErrorStr() << endl;
        return parser; // Return empty parser instead of void
    }

    XMLNode* world = doc.FirstChildElement("world");
    if (!world) {
        cout << "Erro ao carregar o nó raiz 'world' do XML!" << endl;
        return parser; // Return empty parser
    }

    // Carrega as configurações da janela
    XMLElement* win = world->FirstChildElement("window");
	if (!win) {
		cout << "Erro ao carregar o nó 'window' do XML!" << endl;
		return parser;
	}
    parser.win.width = get_int_attribute(win, "width", 800);
    parser.win.width = get_int_attribute(win, "height", 800);
	printf("Tamanho da janela: %d x %d\n", parser.win.width, parser.win.width);

    // Carrega a configuração da câmera
    XMLElement* cam = world->FirstChildElement("camera");
	if (!cam) {
		cout << "Erro ao carregar o nó 'camera' do XML!" << endl;
		return parser;
	}
    cout << "Camera detected!" << endl;
    // Carrega a posição da câmera
    XMLElement* pos = cam->FirstChildElement("position");
    if (pos) {
        parser.cam.px = get_float_attribute(pos, "x", 0);
        parser.cam.py = get_float_attribute(pos, "y", 5);
        parser.cam.pz = get_float_attribute(pos, "z", 0);
    }

    // Carrega a direção para onde a câmera olha
    XMLElement* look = cam->FirstChildElement("lookAt");
    if (look) {
        parser.cam.lx = get_float_attribute(look, "x", 0);
        parser.cam.ly = get_float_attribute(look, "y", 0);
        parser.cam.lz = get_float_attribute(look, "z", 0);
    }

    // Carrega o vetor "up" (direção vertical da câmera)
    XMLElement* up = cam->FirstChildElement("up");
    if (up) {
        parser.cam.ux = get_float_attribute(up, "x", 0);
        parser.cam.uy = get_float_attribute(up, "y", 0);
        parser.cam.uz = get_float_attribute(up, "z", 0);
    }

    // Carrega as configurações de projeção da câmera
    XMLElement* proj = cam->FirstChildElement("projection");
    if (proj) {
        parser.cam.fov = get_float_attribute(proj, "fov", 45);
        parser.cam.near = get_float_attribute(proj, "near", 45);
        parser.cam.far = get_float_attribute(proj, "far", 45);
    }

    // Load Models from groups
    XMLElement* group = world->FirstChildElement("group");
    if (group) {
        cout << "Group detected!" << endl;
        group_xml newGroup; // Create a new group
        
        // Process models in this group
        XMLElement* modelsElement = group->FirstChildElement("models");
        if (modelsElement) {
            XMLElement* modelElement = modelsElement->FirstChildElement("model");
            model_xml model;
            model.file_name = get_string_attribute(modelElement, "file", "default.3d");
            cout << "File detected: " << model.file_name << endl;
            newGroup.models.push_back(model);
            modelElement = modelElement->NextSiblingElement("model");
        }
        
        parser.groups.push_back(newGroup); // Add the group to the parser
    }

    return parser;
}