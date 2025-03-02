#include "figure.hpp"

// Construtor
Figure::Figure()
    : points(), vertices(), normals(), textures(), triangles() {
}

// Destrutor
Figure::~Figure() {
    deleteFigure();
}

// Adiciona um ponto à lista de pontos
void Figure::addPoint(const Point& p) {
    points.add(new Point(p)); // Aloca no heap
}

// Adiciona um vértice
void Figure::addVertex(const Point& p) {
    vertices.add(new Point(p));
}

// Adiciona uma normal
void Figure::addNormal(const Point& p) {
    normals.add(new Point(p));
}

// Adiciona coordenadas de textura
void Figure::addTexture(float u, float v) {
    float* tex = new float[2] { u, v };
    textures.add(tex);
}

// Adiciona um triângulo (índices dos vértices)
void Figure::addTriangle(int v1, int v2, int v3) {
    int* tri = new int[3] { v1, v2, v3 };
    triangles.add(tri);
}

// Retorna referência para a lista de pontos
List<Point>& Figure::getPoints() { return points; }
List<Point>& Figure::getVertices() { return vertices; }
List<Point>& Figure::getNormals() { return normals; }
List<float>& Figure::getTextures() { return textures; }
List<int>& Figure::getTriangles() { return triangles; }

// Salva a figura em um arquivo
void Figure::figureToFile(const char* path) {
    std::ofstream file(path);
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir o arquivo " << path << std::endl;
        return;
    }
    for (long i = 0; i < points.size(); ++i) {
        Point* p = points.get(i);
        file << p->getX() << " " << p->getY() << " " << p->getZ() << std::endl;
    }

    file.close();
}

// Carrega uma figura de um arquivo
Figure Figure::figureFromFile(const char* path) {
    Figure fig;
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir o arquivo " << path << std::endl;
        return fig;
    }

    float x, y, z;
    while (file >> x >> y >> z) {
        fig.addPoint(Point(x, y, z));
    }

    file.close();
    return fig;
}

Figure::Figure(Figure&& other) noexcept {
    points = std::move(other.points);
    vertices = std::move(other.vertices);
    normals = std::move(other.normals);
    textures = std::move(other.textures);
    triangles = std::move(other.triangles);
}

Figure& Figure::operator=(Figure&& other) noexcept {
    if (this != &other) {
        deleteFigure();  // Liberar recursos antes de mover
        points = std::move(other.points);
        vertices = std::move(other.vertices);
        normals = std::move(other.normals);
        textures = std::move(other.textures);
        triangles = std::move(other.triangles);
    }
    return *this;
}

// Deleta a memória usada pela figura
void Figure::deleteFigure() {
    points.deepDeleteList();
    vertices.deepDeleteList();
    normals.deepDeleteList();
    textures.deepDeleteList();
    triangles.deepDeleteList();
}