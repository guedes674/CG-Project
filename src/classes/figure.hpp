#ifndef FIGURE
#define FIGURE

#include "list.hpp"
#include "point.hpp"
#include <cstdio>
#include <fstream>
#include <iostream>

class Figure {
private:
    List points;
    List vertices;
    List normals;
    List textures;
    List triangles;

public:
    Figure();

	~Figure();

    void addPoint(Point p);

    List getPoints();
    
    void figureToFile(const char* path);
    
    static Figure figureFromFile(const char* path);
    
    void deleteFigure();

    void addVertex(Point p);

    void addNormal(Point p);

    void addTexture(float u, float v);

    void addTriangle(int v1, int v2, int v3);

    List getVertices() const;

    List getNormals();

    List getTextures();

    List getTriangles() const;
};

#endif