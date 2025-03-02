#ifndef POINT
#define POINT

#include <cmath>

typedef struct point* Point;

Point point(float x, float y, float z);

float getX(Point point);

float getY(Point point);

float getZ(Point point);

void freeP(Point point);

float calculateDistance(Point point1, Point point2);

Point newPontoSph(float a, float b, float radius);

#endif