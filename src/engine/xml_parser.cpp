#include "xml_parser.hpp"
vector<Figure> models; // modelos carregados a partir do XML


// Função auxiliar para ler um valor float a partir de um atributo
float xml_parser::getFloatAttribute(XMLElement* element, const char* attributeName, float defaultValue) {
    const char* value = element->Attribute(attributeName);
    return value ? stof(value) : defaultValue;
}

// Função auxiliar para ler um valor inteiro a partir de um atributo
int xml_parser::getIntAttribute(XMLElement* element, const char* attributeName, int defaultValue) {
    const char* value = element->Attribute(attributeName);
    return value ? stoi(value) : defaultValue;
}

// Função auxiliar para carregar a configuração da câmera
void xml_parser::loadCameraConfiguration(XMLElement* pCamera) {
    if (!pCamera) return;

    // Carrega a posição da câmera
    XMLElement* pPosition = pCamera->FirstChildElement("position");
    if (pPosition) {
        pos_x = getFloatAttribute(pPosition, "x", pos_x);
        pos_y = getFloatAttribute(pPosition, "y", pos_y);
        pos_z = getFloatAttribute(pPosition, "z", pos_z);
        cradius = sqrt(pos_x * pos_x + pos_y * pos_y + pos_z * pos_z);  // Atualizar o raio da câmera
    }

    // Carrega a direção para onde a câmera olha
    XMLElement* pLookAt = pCamera->FirstChildElement("lookAt");
    if (pLookAt) {
        look_x = getFloatAttribute(pLookAt, "x", look_x);
        look_y = getFloatAttribute(pLookAt, "y", look_y);
        look_z = getFloatAttribute(pLookAt, "z", look_z);
    }

    // Carrega o vetor "up" (direção vertical da câmera)
    XMLElement* pUp = pCamera->FirstChildElement("up");
    if (pUp) {
        up_x = getFloatAttribute(pUp, "x", up_x);
        up_y = getFloatAttribute(pUp, "y", up_y);
        up_z = getFloatAttribute(pUp, "z", up_z);
    }

    // Carrega as configurações de projeção da câmera
    XMLElement* pProjection = pCamera->FirstChildElement("projection");
    if (pProjection) {
        fov = getFloatAttribute(pProjection, "fov", fov);
        near = getFloatAttribute(pProjection, "near", near);
        far = getFloatAttribute(pProjection, "far", far);
    }
}

// Função auxiliar para carregar as configurações da janela a partir do XML
void xml_parser::loadWindowConfiguration(XMLElement* pWindow) {
    if (pWindow) {
        w_width = getIntAttribute(pWindow, "width", w_width);
        w_height = getIntAttribute(pWindow, "height", w_height);
    }
}

// Função auxiliar para carregar os modelos 3D a partir do XML
void xml_parser::loadModels(XMLElement* pModels) {
    if (!pModels) {
        printf("Erro: pModels é nulo\n");
        return;
    }

    XMLElement* pModel = pModels->FirstChildElement("model");

    while (pModel) {
        Figure model_instance;
        const char* modelFile = pModel->Attribute("file");
        if (!modelFile) {
            printf("Erro: modelFile é nulo\n");
            pModel = pModel->NextSiblingElement("model");
            continue;
        }

        printf("Carregando modelo %s...\n", modelFile);
        model_instance = Figure::figureFromFile(modelFile);
        models.push_back(std::move(model_instance));
        printf("Modelo carregado com sucesso!\n");

        pModel = pModel->NextSiblingElement("model");
        if (pModel) {
            printf("Carregando próximo modelo...\n");
        }
    }
}

camera xml_parser::getCamera() {
    return cam;
}

group* xml_parser::getGroup() {
    return grupo;
}

void xml_parser::addModel(Figure model) {
    grupo->addModel(model);
}

void xml_parser::parseCamera(XMLElement * elem) {
    float posX, posY, posZ;
    XMLElement * positionElem = elem->FirstChildElement("position");
    positionElem->QueryFloatAttribute("x", &posX);
    positionElem->QueryFloatAttribute("y", &posY);
    positionElem->QueryFloatAttribute("z", &posZ);
    cam.set_position(new Point(posX, posY, posZ));

    float lookAtX, lookAtY, lookAtZ;
    XMLElement * lookAtElem = elem->FirstChildElement("lookAt");
    lookAtElem->QueryFloatAttribute("x", &lookAtX);
    lookAtElem->QueryFloatAttribute("y", &lookAtY);
    lookAtElem->QueryFloatAttribute("z", &lookAtZ);
    cam.set_lookAt(new Point(lookAtX, lookAtY, lookAtZ));

    float upX, upY, upZ;
    XMLElement * upElem = elem->FirstChildElement("up");
    upElem->QueryFloatAttribute("x", &upX);
    upElem->QueryFloatAttribute("y", &upY);
    upElem->QueryFloatAttribute("z", &upZ);
    cam.set_up(new Point(upX, upY, upZ));

    float fov, near, far;
    XMLElement * projectionElem = elem->FirstChildElement("projection");
    projectionElem->QueryFloatAttribute("fov", &fov);
    projectionElem->QueryFloatAttribute("near", &near);
    projectionElem->QueryFloatAttribute("far", &far);
    cam.set_fov(fov);
    cam.set_near(near);
    cam.set_far(far);
}

void xml_parser::parseGroup(string path, XMLElement * elem, group *g) {
    for(XMLElement * child = elem->FirstChildElement(); child != NULL; child = child->NextSiblingElement()){

        if(strcmp(child->Value(),"models") == 0){// Load models
            parseModels(path, child, g);
        }
        if(strcmp(child->Value(),"group") == 0) {
            group *subGroup = (new group());
            parseGroup(path, child, subGroup);
            g->addGroup(*subGroup);
        }
        if(strcmp(child->Value(),"transform") == 0)
            parseTransform(child, g);
    }

}

void xml_parser::parseModels(string path, XMLElement * elem, group * g) {
    for(XMLElement * child = elem->FirstChildElement(); child != NULL; child = child->NextSiblingElement()){
        if(strcmp(child->Value(),"model") == 0) {
            string filename = child->Attribute("file");
            Figure model;
            cout << path << filename << " model loaded." << endl;
            model.figureFromFile((path + filename).c_str());
            g->addModel(model);
        }
    }
}

void xml_parser::parseTransform(XMLElement * elem, group *g) {
    for(XMLElement * child = elem->FirstChildElement(); child != NULL; child = child->NextSiblingElement()){
        if(strcmp(child->Value(), "rotate") == 0) {
            float angle, x, y, z;
            child->QueryFloatAttribute("angle", &angle);
            child->QueryFloatAttribute("x", &x);
            child->QueryFloatAttribute("y", &y);
            child->QueryFloatAttribute("z", &z);
            g->addRotate(*(new Rotate(angle, x, y, z)));
        } else if(strcmp(child->Value(), "translate") == 0) {
            float x, y, z;
            child->QueryFloatAttribute("x", &x);
            child->QueryFloatAttribute("y", &y);
            child->QueryFloatAttribute("z", &z);
            g->addTranslate(*(new Translate(x, y, z)));
        } else if(strcmp(child->Value(), "scale") == 0) {
            float x, y, z;
            child->QueryFloatAttribute("x", &x);
            child->QueryFloatAttribute("y", &y);
            child->QueryFloatAttribute("z", &z);
            g->addScale(*(new Scale(x, y, z)));
        }
    }
}

// Função para processar o arquivo XML
void xml_parser::xml_parser_function(const char* filename) {
    XMLDocument doc;
    XMLError eResult = doc.LoadFile(filename);

    if (eResult != XML_SUCCESS) {
        cout << "Erro ao carregar arquivo XML: " << doc.ErrorStr() << endl;
        return;
    }

    XMLNode* pRoot = doc.FirstChildElement("world");
    if (!pRoot) {
        cout << "Erro ao carregar o nó raiz 'world' do XML!" << endl;
        return;
    }

    // Carrega as configurações da janela
    XMLElement* pWindow = pRoot->FirstChildElement("window");
	if (!pWindow) {
		cout << "Erro ao carregar o nó 'window' do XML!" << endl;
		return;
	}
    loadWindowConfiguration(pWindow);
	printf("Tamanho da janela: %d x %d\n", w_width, w_height);

    // Carrega a configuração da câmera
    XMLElement* pCamera = pRoot->FirstChildElement("camera");
	if (!pCamera) {
		cout << "Erro ao carregar o nó 'camera' do XML!" << endl;
		return;
	}
    loadCameraConfiguration(pCamera);
	printf("Posição da câmera: (%.2f, %.2f, %.2f)\n", pos_x, pos_y, pos_z);

    // Carrega os modelos 3D
    XMLElement* pGroup = pRoot->FirstChildElement("group");
	printf("Carregando grupos ...\n");
	if (!pGroup) {
		cout << "Erro ao carregar o nó 'group' do XML!" << endl;
		return;
	}

    XMLElement* pTransform = pGroup ? pGroup->FirstChildElement("transform") : nullptr;
    printf("Carregando transform ...\n");


    XMLElement* pModels = pGroup ? pGroup->FirstChildElement("models") : nullptr;
	printf("Carregando modelos 3D...\n");
	if (!pModels) {
		cout << "Erro ao carregar o nó 'models' do XML!" << endl;
		return;
	}
    if (pModels) {
        loadModels(pModels);
    }
}