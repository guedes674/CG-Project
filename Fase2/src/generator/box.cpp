#include "box.hpp"
#include "plane.hpp"

Figure generateBox(float length, int divisions) {
    Figure box = Figure();
    float half = length / 2.0f;
    float step = length / divisions;

    for (int i = 0; i < divisions; i++) {
        for (int j = 0; j < divisions; j++) {
            float x1 = -half + i * step;
            float x2 = -half + (i + 1) * step;
            float y1 = -half + j * step;
            float y2 = -half + (j + 1) * step;

            // Front face (z = half)
            box.addPoint(Point(x1, y1, half));
            box.addPoint(Point(x2, y1, half));
            box.addPoint(Point(x1, y2, half));

            box.addPoint(Point(x1, y2, half));
            box.addPoint(Point(x2, y1, half));
            box.addPoint(Point(x2, y2, half));

            // Back face (z = -half)
            box.addPoint(Point(x1, y1, -half));
            box.addPoint(Point(x1, y2, -half));
            box.addPoint(Point(x2, y1, -half));

            box.addPoint(Point(x1, y2, -half));
            box.addPoint(Point(x2, y2, -half));
            box.addPoint(Point(x2, y1, -half));

            // Left face (x = -half)
            box.addPoint(Point(-half, y1, x1));
            box.addPoint(Point(-half, y2, x1));
            box.addPoint(Point(-half, y1, x2));

            box.addPoint(Point(-half, y2, x1));
            box.addPoint(Point(-half, y2, x2));
            box.addPoint(Point(-half, y1, x2));

            // Right face (x = half)
            box.addPoint(Point(half, y1, x1));
            box.addPoint(Point(half, y1, x2));
            box.addPoint(Point(half, y2, x1));

            box.addPoint(Point(half, y2, x1));
            box.addPoint(Point(half, y1, x2));
            box.addPoint(Point(half, y2, x2));

            // Top face (y = half)
            box.addPoint(Point(x1, half, y1));
            box.addPoint(Point(x1, half, y2));
            box.addPoint(Point(x2, half, y1));

            box.addPoint(Point(x1, half, y2));
            box.addPoint(Point(x2, half, y2));
            box.addPoint(Point(x2, half, y1));

            // Bottom face (y = -half)
            box.addPoint(Point(x1, -half, y1));
            box.addPoint(Point(x2, -half, y1));
            box.addPoint(Point(x1, -half, y2));

            box.addPoint(Point(x1, -half, y2));
            box.addPoint(Point(x2, -half, y1));
            box.addPoint(Point(x2, -half, y2));
        }
    }

    return box;
}