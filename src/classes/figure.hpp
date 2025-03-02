#ifndef FIGURE_HPP
#define FIGURE_HPP

#include <iostream>
#include <fstream>
#include "list.hpp"
#include "point.hpp"

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

    // Proibir cópia para evitar double free
    Figure(const Figure&) = delete;
    Figure& operator=(const Figure&) = delete;

    // Permitir movimentação
    Figure(Figure&& other) noexcept;
    Figure& operator=(Figure&& other) noexcept;

    void addPoint(Point p);
    void addVertex(Point p);
    void addNormal(Point p);
    void addTexture(float u, float v);
    void addTriangle(int v1, int v2, int v3);

    List getPoints() const;
    List getVertices() const;
    List getNormals() const;
    List getTextures() const;
    List getTriangles() const;

    void figureToFile(const char* path);
    static Figure figureFromFile(const char* path);

    void deleteFigure();
};

#endif // FIGURE_HPP