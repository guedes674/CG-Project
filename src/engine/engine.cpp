#include "engine.hpp"

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

void changeSize(int w, int h) {
    if(h == 0) h = 1;

    // computa a razão da janela
    float ratio = w * 1.0 / h;

    // Set the projection matrix as current
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Set viewport
    glViewport(0, 0, w, h);

    // Set perspectiva
    gluPerspective(fov, ratio, near, far);

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
    }
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
        std::vector<Point> vertexes = model.getPoints();

        if (vertexes.empty()) {
            printf("Erro: Lista de vértices é vazia\n");
            continue;
        }

        glBegin(GL_TRIANGLES);
        for (size_t i = 0; i < vertexes.size(); i += 3) {
            Point p1 = vertexes[i];
            Point p2 = vertexes[i + 1];
            Point p3 = vertexes[i + 2];
            glVertex3f(p1.x, p1.y, p1.z);
            glVertex3f(p2.x, p2.y, p2.z);
            glVertex3f(p3.x, p3.y, p3.z);
        }
        glEnd();
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

    // troca buffers
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
    printf("Arquivo XML carregado com sucesso!\n");

    // Inicializar o GLUT
    glutInit(&argc, argv);
    printf("GLUT inicializado\n");
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    printf("Modo de exibição definido\n");
    glutInitWindowPosition(100, 100);
    printf("Posição da janela definida\n");
    glutInitWindowSize(w_width, w_height);
    printf("Tamanho da janela definido\n");
    glutCreateWindow("CG");
    printf("Janela criada\n");

    // Registrar callbacks
    glutDisplayFunc(renderScene);
    printf("Função de exibição registrada\n");
    glutReshapeFunc(changeSize);
    printf("Função de redimensionamento registrada\n");
    glutIdleFunc(renderScene);
    printf("Função de renderização registrada\n");
    glutKeyboardFunc(processKeys);
    printf("Função de processamento de teclas normais registrada\n");
    glutSpecialFunc(processSpecialKeys);
    printf("Função de processamento de teclas especiais registrada\n");

    // Configurações do OpenGL
    glEnable(GL_DEPTH_TEST);
    printf("Profundidade ativada\n");

    // Entrar no loop principal do GLUT
    glutMainLoop();
    printf("Loop principal iniciado\n");

    return 0;
}