#include "plane.hpp"

Figure generatePlane(float length, int divisions) {
    Figure plane = Figure();

    float dimension = length / 2;
    float step = length / divisions;

    for (int line = 0; line < divisions; line++) {
        for (int column = 0; column < divisions; column++) {
            float x1 = -dimension + column * step;
            float z1 = -dimension + line * step;

            float x2 = x1;
            float z2 = z1 + step;

            float x3 = x1 + step;
            float z3 = z1;

            float x4 = x1 + step;
            float z4 = z1 + step;

            plane.addPoint(Point(x1, 0, z1));
            plane.addPoint(Point(x2, 0, z2));
            plane.addPoint(Point(x3, 0, z3));

            plane.addPoint(Point(x2, 0, z2));
            plane.addPoint(Point(x4, 0, z4));
            plane.addPoint(Point(x3, 0, z3));
        }
    }

    return plane;
}