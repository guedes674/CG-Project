#include "point.hpp"

struct point
{
    float x;
    float y;
    float z;
};

//construtor
Point point(float x, float y, float z) {
    Point newP = (Point)malloc(sizeof(Point));
    newP->x = x;
    newP->y = y;
    newP->z = z;
    return newP;
}

//gets
float getX(Point point) {
    if (point) {
        return point->x;
    }
    return 0.0f;
}

float getY(Point point) {
    if (point) {
        return point->y;
    }
    return 0.0f;
}

float getZ(Point point) {
    if (point) {
        return point->z;
    }
    return 0.0f;
}

//free
void freeP(Point point) {
    free(point);
}

//distance
float calculateDistance(Point point1, Point point2) {
    return sqrt(
        pow((point1->x - point2->x), 2) +
        pow((point1->y - point2->y), 2) +
        pow((point1->z - point2->z), 2)
    );
}


//spherical to sartesian coordinate converter
Point newPontoSph(float a, float b, float radius) {
    if (radius < 0.0f) {
        radius = 0.0f;
    }

    float z = radius * cos(b) * cos(a);
    float x = radius * cos(b) * sin(a);
    float y = radius * sin(b);


    return point(x, y, z);
}