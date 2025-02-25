#include "point.hpp"

class Point {
private:
	float x;
	float y;
	float z;
public:
	float getX() {
		return x;
	}

	float getY() {
		return y;
	}

	float getZ() {
		return z;
	}

	float getDistanceToOrigin() {
		return sqrt(x * x + y * y + z * z);
	}

	float getDistance(Point p) {
		float dx = x - p.getX();
		float dy = y - p.getY();
		float dz = z - p.getZ();
		return sqrt(dx * dx + dy * dy + dz * dz);
	}

	void deletePoint() {
		free(this);
	}
};