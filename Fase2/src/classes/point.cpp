#include "point.hpp"

// Construtor padrão
Point::Point() : x(0.0f), y(0.0f), z(0.0f) {}

// Construtor com parâmetros
Point::Point(float x, float y, float z) : x(x), y(y), z(z) {}

// Calcula a distância para outro ponto
float Point::distanceTo(const Point& other) const {
    return std::sqrt(
        (x - other.x) * (x - other.x) +
        (y - other.y) * (y - other.y) +
        (z - other.z) * (z - other.z)
    );
}

// Converte coordenadas esféricas para cartesianas
Point Point::fromSpherical(float a, float b, float radius) {
    if (radius < 0.0f) {
        radius = 0.0f;
    }

    float z = radius * std::cos(b) * std::cos(a);
    float x = radius * std::cos(b) * std::sin(a);
    float y = radius * std::sin(b);

    return Point(x, y, z);
}

Point Point::newPointSph(float a, float b, float radius){
    float z = radius * cos(b) * cos(a);
    float x = radius * cos(b) * sin(a);
    float y = radius * sin(b);

    return Point(x, y, z);
}