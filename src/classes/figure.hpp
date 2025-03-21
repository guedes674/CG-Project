#ifndef FIGURE_HPP
#define FIGURE_HPP

#include <vector>
#include <array>
#include <cstdio>
#include <stdexcept>
#include "point.hpp"

class Figure {
public:
    // Construtores e destrutores
    Figure();
    ~Figure();

    // Funções para adicionar pontos, vértices, normais, texturas e triângulos
    void addPoint(Point p);
    void addVertex(Point p);
    void addNormal(Point p);
    void addTexture(float u, float v);
    void addTriangle(int v1, int v2, int v3);

    // Funções para obter os dados
    const std::vector<Point>& getPoints() const;
    const std::vector<Point>& getVertices() const;
    const std::vector<Point>& getNormals() const;
    const std::vector<std::array<float, 2>>& getTextures() const;
    const std::vector<std::array<int, 3>>& getTriangles() const;

    // Funções de leitura e escrita em arquivo
    void figureToFile(const char* path);
    static Figure figureFromFile(const char* path);

    // Construtor de movimento
    Figure(Figure&& other) noexcept;
    Figure& operator=(Figure&& other) noexcept;

private:
    // Função privada para limpar a figura
    void deleteFigure();

    std::vector<Point> points;
    std::vector<Point> vertices;
    std::vector<Point> normals;
    std::vector<std::array<float, 2>> textures;
    std::vector<std::array<int, 3>> triangles;
};

#endif