#ifndef POINT_HPP
#define POINT_HPP

#include <cmath>

class Point {
public:
    float x, y, z;

    // Construtor padrão
    Point();

    // Construtor com parâmetros
    Point(float x, float y, float z);

    // Calcula a distância para outro ponto
    float distanceTo(const Point& other) const;

    // Converte coordenadas esféricas para cartesianas
    static Point fromSpherical(float a, float b, float radius);

    // Cria um ponto baseado em coordenadas esfericas
    static Point newPointSph(float a, float b, float radius);
};

#endif