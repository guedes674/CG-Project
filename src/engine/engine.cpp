#include "engine.hpp"

using namespace std;
using namespace tinyxml2;

// variáveis globais
int w_width, w_height;                          // Tamanho da janela lido do XML
float pos_x = 5.0, pos_y = 5.0, pos_z = 5.0;    // posição da câmera
float look_x = 0, look_y = 0, look_z = 0;       // orientação da câmera
float up_x = 0, up_y = 1.0, up_z = 0;           // eixo vertical da câmera
float fov = 45, near = 1, far = 1000;           // perspectiva da câmera
float zoomx = 1.0, zoomy = 1.0, zoomz = 1.0;    // zoom da câmera
float cradius = 7.0f;                           // raio da câmera
int mode = GL_LINE, face = GL_FRONT_AND_BACK;   // modo de visualização
vector<Figure> models;                          // modelos carregados a partir do XML

// Função auxiliar para ler um valor float a partir de um atributo
float getFloatAttribute(XMLElement* element, const char* attributeName, float defaultValue) {
    const char* value = element->Attribute(attributeName);
    return value ? stof(value) : defaultValue;
}

// Função auxiliar para ler um valor inteiro a partir de um atributo
int getIntAttribute(XMLElement* element, const char* attributeName, int defaultValue) {
    const char* value = element->Attribute(attributeName);
    return value ? stoi(value) : defaultValue;
}

// Função auxiliar para carregar a configuração da câmera
void loadCameraConfiguration(XMLElement* pCamera) {
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
void loadWindowConfiguration(XMLElement* pWindow) {
    if (pWindow) {
        w_width = getIntAttribute(pWindow, "width", w_width);
        w_height = getIntAttribute(pWindow, "height", w_height);
    }
}

// Função auxiliar para carregar os modelos 3D a partir do XML
void loadModels(XMLElement* pModels) {
    XMLElement* pModel = pModels->FirstChildElement("model");

    while (pModel) {
        Figure model_instance;
        const char* modelFile = pModel->Attribute("file");
        if (modelFile) {
            model_instance = Figure::figureFromFile(modelFile);
            models.push_back(model_instance);
        }
        pModel = pModel->NextSiblingElement("model");
    }
}

// Função para processar o arquivo XML
void xml_parser(const char* filename) {
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
    loadWindowConfiguration(pWindow);

    // Carrega a configuração da câmera
    XMLElement* pCamera = pRoot->FirstChildElement("camera");
    loadCameraConfiguration(pCamera);

    // Carrega os modelos 3D
    XMLElement* pGroup = pRoot->FirstChildElement("group");
    XMLElement* pModels = pGroup ? pGroup->FirstChildElement("models") : nullptr;
    if (pModels) {
        loadModels(pModels);
    }
}

void changeSize(int w, int h) {
    // Prevent a divide by zero, when window is too short
    // (you cant make a window with zero width).
    if(h == 0)
        h = 1;

    // compute window's aspect ratio
    float ratio = w * 1.0 / h;

    // Set the projection matrix as current
    glMatrixMode(GL_PROJECTION);
    // Load Identity Matrix
    glLoadIdentity();

    // Set the viewport to be the entire window
    glViewport(0, 0, w, h);

    // Set perspective
    gluPerspective(fov, ratio, near, far);

    // return to the model view matrix mode
    glMatrixMode(GL_MODELVIEW);
}

void processKeys(unsigned char key, int xx, int yy) {

	switch (key) {
	case 'w':
		pos_z -= 0.1;
		break;
	case 's':
		pos_z += 0.1;
		break;
	case 'a':
		pos_x -= 0.1;
		break;
	case 'd':
		pos_x += 0.1;
		break;
    case '+':
		zoomx += 0.1;
		zoomy += 0.1;
		zoomz += 0.1;
		break;
	case '-':
		zoomx -= 0.1;
		zoomy -= 0.1;
		zoomz -= 0.1;
		break;

    glutPostRedisplay();
}

void processSpecialKeys(int key, int xx, int yy) {

	switch (key) {
	case GLUT_KEY_RIGHT:
		look_x += 0.1;
		break;
	case GLUT_KEY_LEFT:
		look_x -= 0.1;
		break;
	case GLUT_KEY_UP:
		look_y += 0.1;
		break;
	case GLUT_KEY_DOWN:
		look_y -= 0.1;
		break;
	}

    glutPostRedisplay();
}

// Função para desenhar os modelos 3D carregados
void drawScene() {
    for (const auto& model : models) {
        for (const auto& triangle : model.getTriangles()) {
            glBegin(GL_TRIANGLES);
            for (int i = 0; i < 3; ++i) {
                Point* p = (Point*)model.getVertices().get(triangle[i]);
                glVertex3f(p->getX(), p->getY(), p->getZ());
            }
            glEnd();
        }
    }
}

// Função de renderização principal
void renderScene(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // atualiza a posição da câmera
    pos_x = cradius * cos(look_y) * sin(look_x);
    pos_y = cradius * sin(look_y); 
    pos_z = cradius * cos(look_y) * cos(look_x);

    // define a câmera
    glLoadIdentity();
    gluLookAt(pos_x, pos_y, pos_z, look_x, look_y, look_z, up_x, up_y, up_z);
	glScalef(zoomx, zoomy, zoomz);

    // define modo de polígonos
    glPolygonMode(GL_FRONT_AND_BACK, mode);

    // desenha os modelos
    drawScene();

    // trroca buffers
    glutSwapBuffers();
}

// Função para inicializar o GLUT
int main(int argc, char** argv) {
    if (argc != 2) {
        cout << "Uso: " << argv[0] << " <arquivo_xml>" << endl;
        return 1;
    }

    // Carregar o arquivo XML
    xml_parser(argv[1]);

    // Inicializar o GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(w_width, w_height);
    glutCreateWindow("CG");

    // Registrar callbacks
    glutDisplayFunc(renderScene);
    glutReshapeFunc(changeSize);
	glutIdleFunc(renderScene);
    glutKeyboardFunc(processKeys);
    glutSpecialFunc(processSpecialKeys);

    // Configurações do OpenGL
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // Entrar no loop principal do GLUT
    glutMainLoop();

    return 0;
}