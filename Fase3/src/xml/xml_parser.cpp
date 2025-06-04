#include "xml_parser.h"

float get_float_attribute(XMLElement* element, const char* attribute_name, float defaultValue) {
    const char* value = element->Attribute(attribute_name);
    return value ? stof(value) : defaultValue;
}

int get_int_attribute(XMLElement* element, const char* attribute_name, int defaultValue) {
    const char* value = element->Attribute(attribute_name);
    return value ? stoi(value) : defaultValue;
}

string get_string_attribute(XMLElement* element, const char* attribute_name, string defaultValue) {
    const char* value = element->Attribute(attribute_name);
    return value ? string(value) : defaultValue;
}

group_xml recursive_xml_catch(string file_name){
    group_xml output_group;
    cout << "Reading file :" << file_name << endl;
    XMLDocument doc;
    XMLError eResult = doc.LoadFile(file_name.c_str());

    if (eResult != XML_SUCCESS) {
        cout << "Erro ao carregar arquivo XML recursivo: " << doc.ErrorStr() << endl;
        return output_group; // Return empty group instead of void
    }

    XMLNode* world = doc.FirstChildElement("world");
    if (!world) {
        cout << "Erro ao carregar o nó raiz 'world' do XML!" << endl;
        return output_group; // Return empty group
    }

    // Load Models from groups
    XMLElement* group = world->FirstChildElement("group");
    while (group) {
        group_xml new_group = recursive_catch_groups(group);
        output_group.groups.push_back(new_group);
        group = group->NextSiblingElement("group");
    }

    return output_group;
}

group_xml recursive_catch_groups(XMLElement* group) {
    group_xml new_group;
    XMLElement* models_element = group->FirstChildElement("models");
    if (models_element) {
        XMLElement* model_element = models_element->FirstChildElement("model");
        while (model_element) {
            model_xml model;
            model.file_name = get_string_attribute(model_element, "file", "default.3d");
            new_group.models.push_back(model);
            model_element = model_element->NextSiblingElement("model");
        }
        
    }
    XMLElement* transform_element = group->FirstChildElement("transform");
    if(transform_element){
        // Start with the first child element and iterate through all siblings
        XMLElement* current_transform = transform_element->FirstChildElement();
        int order_index = 0;
        
        while(current_transform) {
            const char* transform_type = current_transform->Name();
            if(strcmp(transform_type, "rotate") == 0) {
                // Process rotation
                new_group.transformations.rotation.time = get_int_attribute(current_transform, "time",0);
                // always read angle (default to 360° per cycle if unspecified)
                new_group.transformations.rotation.angle = get_float_attribute(current_transform, "angle", 360.0f);
                new_group.transformations.rotation.x = get_float_attribute(current_transform, "x", 0);
                new_group.transformations.rotation.y = get_float_attribute(current_transform, "y", 0);
                new_group.transformations.rotation.z = get_float_attribute(current_transform, "z", 0);
                new_group.transformations.rotation.order = order_index++;
                new_group.transformations.rotation_exists++;
            } else if(strcmp(transform_type, "translate") == 0) {
                // Process translation
                new_group.transformations.translation.time = get_int_attribute(current_transform, "time",0);
                if (new_group.transformations.translation.time == 0){
                    new_group.transformations.translation.x = get_float_attribute(current_transform, "x", 0);
                    new_group.transformations.translation.y = get_float_attribute(current_transform, "y", 0);
                    new_group.transformations.translation.z = get_float_attribute(current_transform, "z", 0);
                }
                else{
                    string align      = get_string_attribute(current_transform, "align",    "false");
                    string tracking   = get_string_attribute(current_transform, "tracking", "false");

                    XMLElement* point_xml = current_transform->FirstChildElement("point");
                    vector<Vector3> points;
                    while (point_xml){
                        float x = get_float_attribute(point_xml, "x", 0);
                        float y = get_float_attribute(point_xml, "y", 0);
                        float z = get_float_attribute(point_xml, "z", 0);
                        points.push_back(Vector3(x,y,z));
                        point_xml = point_xml->NextSiblingElement("point");
                    }
                    bool align_flag    = strcmp(align.c_str(), "true") == 0;
                    bool tracking_flag = strcmp(tracking.c_str(), "true") == 0;
                    time_transformation_xml time_trans(align_flag, tracking_flag, points);
                    new_group.transformations.translation.time_trans = time_trans;
                }
                new_group.transformations.translation.order = order_index++;
                new_group.transformations.translation_exists++;
                
            } else if(strcmp(transform_type, "scale") == 0) {
                // Process scale
                new_group.transformations.scale.x = get_float_attribute(current_transform, "x", 1);
                new_group.transformations.scale.y = get_float_attribute(current_transform, "y", 1);
                new_group.transformations.scale.z = get_float_attribute(current_transform, "z", 1);
                new_group.transformations.scale.order = order_index++;
                new_group.transformations.scale_exists++;
            }
            
            // Move to the next sibling element
            current_transform = current_transform->NextSiblingElement();
        }
    }

    XMLElement* xmls_element = group->FirstChildElement("xmls");
    if(xmls_element){
        XMLElement* xml_element = xmls_element->FirstChildElement("xml");
        while (xml_element) {
            group_xml xml;
            string xml_file = get_string_attribute(xml_element, "file", "");
            xml = recursive_xml_catch(xml_file);
            new_group.groups.push_back(xml);
            xml_element = xml_element->NextSiblingElement("xml");
        }
    }

    XMLElement* group_element = group->FirstChildElement("group");
    while (group_element) {
        group_xml subGroup = recursive_catch_groups(group_element);
        new_group.groups.push_back(subGroup);
        group_element = group_element->NextSiblingElement("group");
    }
    return new_group;
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

    // Carrega a configuração da câmera
    XMLElement* cam = world->FirstChildElement("camera");
	if (!cam) {
		cout << "Erro ao carregar o nó 'camera' do XML!" << endl;
		return parser;
	}
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
    while (group) {
        group_xml new_group = recursive_catch_groups(group);
        parser.groups.push_back(new_group);
        group = group->NextSiblingElement("group");
    }

    return parser;
}