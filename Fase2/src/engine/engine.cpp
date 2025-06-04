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
xml_parser parser;

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

void transforms(group grupo){
    vector<Rotate> rotates = grupo.getRotate();
    if(!rotates.empty())
        for(Rotate rotate : rotates)
            glRotatef(rotate.get_angle(), rotate.get_x(), rotate.get_y(), rotate.get_z());

    vector<Translate> translates = grupo.getTranslate();
    if(!translates.empty())
        for(Translate translate : translates)
            glTranslatef(translate.get_x(), translate.get_y(), translate.get_z());

    vector<Scale> scales = grupo.getScale();
    if(!scales.empty())
        for(Scale scale : scales)
            glScalef(scale.get_x(), scale.get_y(), scale.get_z());
}

void renderModels(group grupo) {
    glPushMatrix();

    transforms(grupo);

    vector<Figure> models = grupo.getModels();

    vector<group> subGroups = grupo.getGroups();
    if (!subGroups.empty()) {
        for (group g: subGroups)
            renderModels(g);
    }
    glPopMatrix();
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

    renderModels(*parser.getGroup());

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
    xml_parser parser;
    parser.xml_parser_function(argv[1]);

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