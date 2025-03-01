#include "figure.hpp"

Figure::Figure() {
	points = List();
	vertices = List();
	normals = List();
	textures = List();
	triangles = List();
}

Figure::~Figure() {
    deleteFigure();
}

void Figure::addPoint(Point p) {
    points.add(new Point(p));
}

List Figure::getPoints() {
    return points;
}

void Figure::addVertex(Point p) {
    vertices.add(new Point(p));
}

void Figure::addNormal(Point p) {
    normals.add(new Point(p));
}

void Figure::addTexture(float u, float v) {
    float* tex = new float[2]{ u, v };
    textures.add(tex);
}

void Figure::addTriangle(int v1, int v2, int v3) {
    int* tri = new int[3]{ v1, v2, v3 };
    triangles.add(tri);
}

List Figure::getVertices() const{
    return vertices;
}

List Figure::getNormals() {
    return normals;
}

List Figure::getTextures() {
    return textures;
}

List Figure::getTriangles() const{
    return triangles;
}

void Figure::figureToFile(const char* path) {
    std::ofstream file(path);
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir o arquivo " << path << std::endl;
        return;
    }

    for (long i = 0; i < points.size(); ++i) {
        Point* p = (Point*)points.get(i);
        file << p->getX() << " " << p->getY() << " " << p->getZ() << std::endl;
    }

    file.close();
}

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

void Figure::deleteFigure() {
    if (!points.size()) return;
    points.deepDeleteList();
    vertices.deepDeleteList();
    normals.deepDeleteList();
    textures.deepDeleteList();
    triangles.deepDeleteList();
}