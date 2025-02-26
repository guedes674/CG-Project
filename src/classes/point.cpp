#include "point.hpp"

class Point {
private:
	float x;
	float y;
	float z;
public:
	Point() {
		x = 0;
		y = 0;
		z = 0;
	}

	Point(float x, float y, float z) {
		this.x = x;
		this.y = y;
		this.z = z;
	}

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