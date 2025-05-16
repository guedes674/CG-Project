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

lights_xml catch_lights(XMLElement* lights_element){
    lights_xml lights;
    XMLElement* light_xml = lights_element->FirstChildElement("light");
    while (light_xml){
        string type = get_string_attribute(light_xml, "type", "");
        if (type != ""){
            if (type == "point"){
                //<light type="point" posx="0" posy="0.2" posz="0" />

                light current_light;

                current_light.x = get_float_attribute(light_xml, "posx" ,0.0f);
                current_light.y = get_float_attribute(light_xml, "posy" ,0.0f);
                current_light.z = get_float_attribute(light_xml, "posz" ,0.0f);
                current_light.type = 0;

                lights.lights.push_back(current_light);

                cout << "Light point - x : " << current_light.x << " y : " << current_light.y << " z : " << current_light.z << endl;
            }
            else if (type == "directional"){
                //<light type="directional" dirx="1" diry="0.7" dirz="0.5"/>

                light current_light;

                current_light.x = get_float_attribute(light_xml, "dirx" ,0.0f);
                current_light.y = get_float_attribute(light_xml, "diry" ,0.0f);
                current_light.z = get_float_attribute(light_xml, "dirz" ,0.0f);
                current_light.type = 1;

                lights.lights.push_back(current_light);
                cout << "Light direction - x : " << current_light.x << " y : " << current_light.y << " z : " << current_light.z << endl;
            }
            else if (type == "spot"){
                //<light type="spot" posx="0" posy="2" posz="4" dirx="0" diry="-2" dirz="-4" cutoff="10"/>

                light_spot current_light_spot;

                light point;
                
                point.x = get_float_attribute(light_xml, "posx" ,0.0f);
                point.y = get_float_attribute(light_xml, "posy" ,0.0f);
                point.z = get_float_attribute(light_xml, "posz" ,0.0f);
                point.type = 0;

                light direction;

                direction.x = get_float_attribute(light_xml, "dirx" ,0.0f);
                direction.y = get_float_attribute(light_xml, "diry" ,0.0f);
                direction.z = get_float_attribute(light_xml, "dirz" ,0.0f);
                direction.type = 1;

                current_light_spot.cutoff = get_float_attribute(light_xml, "cutoff" , 0.0f);

                current_light_spot.point = point;
                current_light_spot.direction = direction;

                lights.light_spots.push_back(current_light_spot);
            }
            else{
                cout << "Invalid light type found : " << type << endl;
            }
        }
        light_xml = light_xml->NextSiblingElement("light");
    }
    return lights;
}

group_xml recursive_catch_groups(XMLElement* group) {
    group_xml new_group;
    XMLElement* models_element = group->FirstChildElement("models");
    if (models_element) {
        XMLElement* model_element = models_element->FirstChildElement("model");
        while (model_element) {

            model_xml model;
            string file_name;
            string texture_name;

            file_name = get_string_attribute(model_element, "file", "");
            XMLElement* texture_element = model_element->FirstChildElement("texture");

            if (texture_element) {
                texture_name = get_string_attribute(texture_element, "file", "");
            } else {
                texture_name = "";
            } 
            XMLElement* color_element = model_element->FirstChildElement("color");
            if (color_element){

                XMLElement* diffuse_element = color_element->FirstChildElement("diffuse");
                int diffuse_r = get_int_attribute(diffuse_element, "R", 0);
                int diffuse_g = get_int_attribute(diffuse_element, "G", 0);
                int diffuse_b = get_int_attribute(diffuse_element, "B", 0);

                XMLElement* ambient_element = color_element->FirstChildElement("ambient");
                int ambient_r = get_int_attribute(ambient_element, "R", 0);
                int ambient_g = get_int_attribute(ambient_element, "G", 0);
                int ambient_b = get_int_attribute(ambient_element, "B", 0);

                XMLElement* specular_element = color_element->FirstChildElement("specular");
                int specular_r = get_int_attribute(specular_element, "R", 0);
                int specular_g = get_int_attribute(specular_element, "G", 0);
                int specular_b = get_int_attribute(specular_element, "B", 0);

                XMLElement* emissive_element = color_element->FirstChildElement("emissive");
                int emissive_r = get_int_attribute(emissive_element, "R", 0);
                int emissive_g = get_int_attribute(emissive_element, "G", 0);
                int emissive_b = get_int_attribute(emissive_element, "B", 0);

                XMLElement* shininess_element = color_element->FirstChildElement("shininess");
                int shininess_value = get_int_attribute(shininess_element, "value", 0);

                color model_color(diffuse_r, diffuse_g, diffuse_b,
                    ambient_r, ambient_g, ambient_b,
                    specular_r, specular_g, specular_b,
                    emissive_r, emissive_g, emissive_b,
                    shininess_value);

                model = model_xml(file_name, texture_name, model_color);
            }


            else{
                color model_color;
                model = model_xml(file_name, texture_name, model_color);
            }
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

    // Carrega as configurações das luzes
    XMLElement* lights_element = world->FirstChildElement("lights");
    if (lights_element){
        parser.lights = catch_lights(lights_element);
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