#ifndef POINT_HPP
#define POINT_HPP

#include <cmath>

class Point {
private:
    float x, y, z;

public:
    Point();
    Point(float x, float y, float z);

    float getX() const;
    float getY() const;
    float getZ() const;
    
    float getDistanceToOrigin() const;
    float getDistance(const Point& p) const;
};

#endif
