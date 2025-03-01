#include "point.hpp"

Point::Point() : x(0), y(0), z(0) {}

Point::Point(float x, float y, float z) : x(x), y(y), z(z) {}

float Point::getX() const { return x; }
float Point::getY() const { return y; }
float Point::getZ() const { return z; }

float Point::getDistanceToOrigin() const {
    return sqrt(x * x + y * y + z * z);
}

float Point::getDistance(const Point& p) const {
    float dx = x - p.getX();
    float dy = y - p.getY();
    float dz = z - p.getZ();
    return sqrt(dx * dx + dy * dy + dz * dz);
}

Point::~Point() {}