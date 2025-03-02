#ifndef FIGURE_HPP
#define FIGURE_HPP

#include <iostream>
#include <fstream>
#include "list.hpp"
#include "point.hpp"

class Figure {
private:
    List<Point> points;
    List<Point> vertices;
    List<Point> normals;
    List<float> textures;
    List<int> triangles;

public:
    Figure();
    ~Figure();

    // Proibir cópia para evitar double free
    Figure(const Figure&) = delete;
    Figure& operator=(const Figure&) = delete;

    // Permitir movimentação
    Figure(Figure&& other) noexcept;
    Figure& operator=(Figure&& other) noexcept;

    void addPoint(const Point& p);
    void addVertex(const Point& p);
    void addNormal(const Point& p);
    void addTexture(float u, float v);
    void addTriangle(int v1, int v2, int v3);

    List<Point>& getPoints();
    List<Point>& getVertices();
    List<Point>& getNormals();
    List<float>& getTextures();
    List<int>& getTriangles();

    void figureToFile(const char* path);
    static Figure figureFromFile(const char* path);

    void deleteFigure();
};

#endif