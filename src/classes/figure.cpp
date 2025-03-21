#include "figure.hpp"

// Construtor
Figure::Figure() {}

// Destrutor
Figure::~Figure() {
    deleteFigure();
}

// Adiciona um ponto à lista de pontos
void Figure::addPoint(Point p) {
    points.push_back(p);
}

// Adiciona um vértice à lista de vértices
void Figure::addVertex(Point p) {
    vertices.push_back(p);
}

// Adiciona uma normal à lista de normais
void Figure::addNormal(Point p) {
    normals.push_back(p);
}

// Adiciona coordenadas de uma textura à lista de texturas
void Figure::addTexture(float u, float v) {
    textures.push_back({u, v});
}

// Adiciona um triângulo à lista de triângulos
void Figure::addTriangle(int v1, int v2, int v3) {
    triangles.push_back({v1, v2, v3});
}

// Retorna a lista de pontos
const std::vector<Point>& Figure::getPoints() const {
    return points;
}

// Retorna a lista de vértices
const std::vector<Point>& Figure::getVertices() const {
    return vertices;
}

// Retorna a lista de normais
const std::vector<Point>& Figure::getNormals() const {
    return normals;
}

// Retorna a lista de texturas
const std::vector<std::array<float, 2>>& Figure::getTextures() const {
    return textures;
}

// Retorna a lista de triângulos
const std::vector<std::array<int, 3>>& Figure::getTriangles() const {
    return triangles;
}

// Guarda a figura no arquivo
void Figure::figureToFile(const char* path) {
    if (path == nullptr) {
        printf("Impossível criar ficheiro devido a argumentos inválidos\n");
        return;
    }

    FILE* file = fopen(path, "w");
    if (!file) {
        printf("Ocorreu um erro na abertura do ficheiro '%s'\n", path);
        return;
    }

    fprintf(file, "%lu\n", points.size());

    for (const auto& p : points) {
        fprintf(file, "%.3f,%.3f,%.3f\n", p.x, p.y, p.z);
    }
    fclose(file);
}

// Carrega a figura de um arquivo
Figure Figure::figureFromFile(const char* path) {
    Figure fig;
    FILE* file = fopen(path, "r");

    if (!file) {
        printf("Impossível carregar figura devido a argumentos inválidos\n");
        return fig;
    }

    int vertexCount;
    if (fscanf(file, "%d", &vertexCount) != 1) {
        printf("Erro ao ler o número de vértices\n");
        fclose(file);
        return fig;
    }

    float x, y, z;
    for (int i = 0; i < vertexCount; i++) {
        if (fscanf(file, "%f,%f,%f", &x, &y, &z) == 3) {
            fig.addPoint(Point(x, y, z));
        } else {
            printf("Erro ao ler o vértice %d\n", i);
        }
    }
    fclose(file);
    return fig;
}

// Construtor de movimento
Figure::Figure(Figure&& other) noexcept
    : points(std::move(other.points)),
      vertices(std::move(other.vertices)),
      normals(std::move(other.normals)),
      textures(std::move(other.textures)),
      triangles(std::move(other.triangles)) {
}

// Operador de atribuição de movimento
Figure& Figure::operator=(Figure&& other) noexcept {
    if (this != &other) {
        deleteFigure();

        points = std::move(other.points);
        vertices = std::move(other.vertices);
        normals = std::move(other.normals);
        textures = std::move(other.textures);
        triangles = std::move(other.triangles);
    }
    return *this;
}

// Elimina a memória usada pela figura
void Figure::deleteFigure() {
    printf("Deletando figura\n");
}