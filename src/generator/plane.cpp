#include "plane.hpp"

Figure generatePlaneXZ(float length, int divisions, int height) {
    Figure plane = Figure();

    float dimension = length / 2.0;
    float step = length / divisions;

    for (float line = 0.0; line < divisions; line++) {
        for (float column = 0.0; column < divisions; column++) {
            float x1 = -dimension + column * step;
            float z1 = -dimension + line * step;

            float x2 = x1;
            float z2 = z1 + step;

            float x3 = x1 + step;
            float z3 = z1;

            float x4 = x1 + step;
            float z4 = z1 + step;

            plane.addPoint(point(x1, height, z1));
            plane.addPoint(point(x3, height, z3));
            plane.addPoint(point(x2, height, z2));
            plane.addPoint(point(x1, height, z1));
            plane.addPoint(point(x3, height, z3));
            plane.addPoint(point(x2, height, z2));
            plane.addPoint(point(x4, height, z4));
            plane.addPoint(point(x3, height, z3));
            plane.addPoint(point(x4, height, z4));
        }
    }
    return plane;
}

Figure generatePlaneXY(float length, int divisions, int height) {
    Figure plane = Figure();

    float dimension = length / 2.0;
    float step = length / divisions;

    for (float line = 0.0; line < divisions; line++) {
        for (float column = 0.0; column < divisions; column++) {
            float x1 = -dimension + column * step;
            float y1 = -dimension + line * step;

            float x2 = x1;
            float y2 = y1 + step;

            float x3 = x1 + step;
            float y3 = y1;

            float x4 = x1 + step;
            float y4 = y1 + step;

            plane.addPoint(point(x1, y1, height));
            plane.addPoint(point(x3, y3, height));
            plane.addPoint(point(x2, y2, height));
            plane.addPoint(point(x1, y1, height));
            plane.addPoint(point(x3, y3, height));
            plane.addPoint(point(x2, y2, height));
            plane.addPoint(point(x4, y4, height));
            plane.addPoint(point(x3, y3, height));
            plane.addPoint(point(x4, y4, height));
            
        }
    }
    return plane;
}

Figure generatePlaneYZ(float length, int divisions, int height) {
    Figure plane = Figure();

    float dimension = length / 2.0;
    float step = length / divisions;

    for (float line = 0.0; line < divisions; line++) {
        for (float column = 0.0; column < divisions; column++) {
            float y1 = -dimension + column * step;
            float z1 = -dimension + line * step;

            float y2 = y1;
            float z2 = z1 + step;

            float y3 = y1 + step;
            float z3 = z1;

            float y4 = y1 + step;
            float z4 = z1 + step;

            plane.addPoint(point(height, y1, z1));
            plane.addPoint(point(height, y3, z3));
            plane.addPoint(point(height, y2, z2));
            plane.addPoint(point(height, y1, z1));
            plane.addPoint(point(height, y3, z3));
            plane.addPoint(point(height, y2, z2));
            plane.addPoint(point(height, y4, z4));
            plane.addPoint(point(height, y3, z3));
            plane.addPoint(point(height, y4, z4));
            
        }
    }
    return plane;
}