#include <cmath>

class Point {
private:
    float x, y, z;

public:
    Point() : x(0), y(0), z(0) {}

    Point(float x, float y, float z) : x(x), y(y), z(z) {}

    float getX() const { return x; }
    float getY() const { return y; }
    float getZ() const { return z; }

    float getDistanceToOrigin() const {
        return sqrt(x * x + y * y + z * z);
    }

    float getDistance(const Point& p) const {
        float dx = x - p.getX();
        float dy = y - p.getY();
        float dz = z - p.getZ();
        return sqrt(dx * dx + dy * dy + dz * dz);
    }

    ~Point() {
    }
};
