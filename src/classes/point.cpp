#include "point.hpp"

struct ponto {
	float x;
	float y;
	float z;
};

Point newEmptyPoint() {
	Point p = (Point)malloc(sizeof(struct ponto));
	if (p != NULL) {
		p->x = 0.0f;
		p->y = 0.0f;
		p->z = 0.0f;
	}
	return p;
}

Point newPoint(float x, float y, float z) {
	Point p = (Point)malloc(sizeof(struct ponto));
	if (p != NULL) {
		p->x = x;
		p->y = y;
		p->z = z;
	}
	return p;
}

Point newPointSph(float a, float b, float radius) {
	float z = radius * cos(b) * cos(a);
	float x = radius * cos(b) * sin(a);
	float y = radius * sin(b);

	return newPoint(x, y, z);
}

float getX(Point p) {
	if (p) {
		return p->x;
	}
	return 0.0f;
}

float getY(Point p) {
	if (p) {
		return p->y;
	}
	return 0.0f;
}

float getZ(Point p) {
	if (p) {
		return p->z;
	}
	return 0.0f;
}

float getDistanceToOrigin(Point p) {
	return sqrt(p->x * p->x + p->y * p->y + p->z * p->z);
}

float getDistance(Point p1, Point p2) {
	float dx = p1->x - p2->x;
	float dy = p1->y - p2->y;
	float dz = p1->z - p2->z;
	return sqrt(dx * dx + dy * dy + dz * dz);
}

void deletePoint(Point p) {
	free(p);
}