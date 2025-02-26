#include "figure.hpp"

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

List Figure::getVertices() {
    return vertices;
}

List Figure::getNormals() {
    return normals;
}

List Figure::getTextures() {
    return textures;
}

List Figure::getTriangles() {
    return triangles;
}

void Figure::figureToFile(const char* path) {
    FILE* file = fopen(path, "w");
    if (!file) {
        printf("Error opening file\n");
        return;
    }

    fprintf(file, "%ld\n", vertices.size());
    for (long i = 0; i < vertices.size(); i++) {
        Point* p = (Point*)vertices.get(i);
        fprintf(file, "%f,%f,%f\n", p->getX(), p->getY(), p->getZ());
    }

    fclose(file);
}

Figure Figure::figureFromFile(const char* path) {
    FILE* file = fopen(path, "r");
    if (!file) {
        printf("Error opening file\n");
        return Figure();
    }

    Figure fig;
    int count;
    fscanf(file, "%d", &count);
    for (int i = 0; i < count; i++) {
        float x, y, z;
        fscanf(file, "%f,%f,%f", &x, &y, &z);
        fig.addVertex(Point(x, y, z));
    }

    fclose(file);
    return fig;
}

void Figure::deleteFigure() {
    points.deepDeleteList();
    vertices.deepDeleteList();
    normals.deepDeleteList();
    textures.deepDeleteList();
    triangles.deepDeleteList();
}
