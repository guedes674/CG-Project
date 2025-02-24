#ifndef POINT
#define POINT
#include <stdlib.h>
#include <math.h>

typedef struct point* Point;

Point newEmptyPoint();

Point newPoint(float x, float y, float z);

Point newPointSph(float a, float b, float radius);

float getX(Point p);

float getY(Point p);

float getZ(Point p);

float getDistanceToOrigin(Point p);

float getDistance(Point p1, Point p2);

void deletePoint(Point p);

#endif