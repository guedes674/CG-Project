#ifndef FIGURE_HPP
#define FIGURE_HPP

#include "list.hpp"
#include "point.hpp"
#include <cstdio>

class Figure {
private:
    List points;
    List vertices;
    List normals;
    List textures;
    List triangles;

public:
    Figure() = default;

    void addPoint(Point p);

    List getPoints();
    
    void figureToFile(const char* path);
    
    static Figure figureFromFile(const char* path);
    
    void deleteFigure();

    void addVertex(Point p);

    void addNormal(Point p);

    void addTexture(float u, float v);

    void addTriangle(int v1, int v2, int v3);

    List getVertices();

    List getNormals();

    List getTextures();

    List getTriangles();
};

#endif
