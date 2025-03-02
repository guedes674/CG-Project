#include "cone.hpp"
#include <cmath>

Figure generateCone(float radius, float height, int divisions) {
    Figure cone = Figure();
    float halfHeight = height / 2.0f;
    float angleStep = 2.0f * M_PI / divisions;

    // Cone tip
    point tip(0, halfHeight, 0);

    // Generate base and sides
    for (int i = 0; i < divisions; i++) {
        float angle1 = i * angleStep;
        float angle2 = (i + 1) * angleStep;

        float x1 = radius * cos(angle1);
        float z1 = radius * sin(angle1);
        float x2 = radius * cos(angle2);
        float z2 = radius * sin(angle2);

        point p1(x1, -halfHeight, z1);
        point p2(x2, -halfHeight, z2);
        point center(0, -halfHeight, 0);

        // Base triangle (counter-clockwise)
        cone.addPoint(center);
        cone.addPoint(p2);
        cone.addPoint(p1);

        // Side triangle (counter-clockwise)
        cone.addPoint(p1);
        cone.addPoint(p2);
        cone.addPoint(tip);
    }

    return cone;
}
