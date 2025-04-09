#include "xml_parser.h"

int get_float_attribute(XMLElement* element, const char* attributeName, float defaultValue) {
    const char* value = element->Attribute(attributeName);
    return value ? stof(value) : defaultValue;
}

int get_int_attribute(XMLElement* element, const char* attributeName, int defaultValue) {
    const char* value = element->Attribute(attributeName);
    return value ? stoi(value) : defaultValue;
}

xmlparser read_xml_file(string file_name){
    xmlparser parser;
    XMLDocument doc;
    XMLError eResult = doc.LoadFile(file_name.c_str());

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
    parser.win.width = get_int_attribute(pWindow, "width", 800);
    parser.win.width = get_int_attribute(pWindow, "height", 800);
	printf("Tamanho da janela: %d x %d\n", parser.win.width, parser.win.width);

    // Carrega a configuração da câmera
    XMLElement* pCamera = pRoot->FirstChildElement("camera");
	if (!pCamera) {
		cout << "Erro ao carregar o nó 'camera' do XML!" << endl;
		return;
	}

    // Carrega a posição da câmera
    XMLElement* pPosition = pCamera->FirstChildElement("position");
    if (pPosition) {
        parser.cam.px = get_float_attribute(pPosition, "x", 0);
        parser.cam.py = get_float_attribute(pPosition, "y", 5);
        parser.cam.pz = get_float_attribute(pPosition, "z", 0);
    }

    // Carrega a direção para onde a câmera olha
    XMLElement* pLookAt = pCamera->FirstChildElement("lookAt");
    if (pLookAt) {
        parser.cam.lx = get_float_attribute(pLookAt, "x", 0);
        parser.cam.ly = get_float_attribute(pLookAt, "y", 0);
        parser.cam.lz = get_float_attribute(pLookAt, "z", 0);
    }

    // Carrega o vetor "up" (direção vertical da câmera)
    XMLElement* pUp = pCamera->FirstChildElement("up");
    if (pUp) {
        parser.cam.ux = get_float_attribute(pUp, "x", 0);
        parser.cam.uy = get_float_attribute(pUp, "y", 0);
        parser.cam.uz = get_float_attribute(pUp, "z", 0);
    }

    // Carrega as configurações de projeção da câmera
    XMLElement* pProjection = pCamera->FirstChildElement("projection");
    if (pProjection) {
        parser.cam.fov = get_float_attribute(pProjection, "fov", 45);
        parser.cam.near = get_float_attribute(pProjection, "near", 45);
        parser.cam.far = get_float_attribute(pProjection, "far", 45);
    }

}