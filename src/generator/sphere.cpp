#include "sphere.hpp"
#include <cmath>

#define PI 3.14159265359

Figure sphere(float radius, int slices, int stacks) {
    Figure fig;
    fig = Figure();

    float stackStep = PI / stacks;
    float sliceStep = 2 * PI / slices;

    for (int i = 0; i < stacks; i++) {
        float phi1 = i * stackStep - PI / 2;
        float phi2 = (i + 1) * stackStep - PI / 2;

        for (int j = 0; j < slices; j++) {
            float theta1 = j * sliceStep;
            float theta2 = (j + 1) * sliceStep;

            float x1 = radius * cos(phi1) * cos(theta1);
            float y1 = radius * sin(phi1);
            float z1 = radius * cos(phi1) * sin(theta1);

            float x2 = radius * cos(phi1) * cos(theta2);
            float y2 = radius * sin(phi1);
            float z2 = radius * cos(phi1) * sin(theta2);

            float x3 = radius * cos(phi2) * cos(theta1);
            float y3 = radius * sin(phi2);
            float z3 = radius * cos(phi2) * sin(theta1);

            float x4 = radius * cos(phi2) * cos(theta2);
            float y4 = radius * sin(phi2);
            float z4 = radius * cos(phi2) * sin(theta2);

            if (i != 0) {
                fig.addVertex(x1, y1, z1);
                fig.addVertex(x3, y3, z3);
                fig.addVertex(x4, y4, z4);

                fig.addNormal(x1 / radius, y1 / radius, z1 / radius);
                fig.addNormal(x3 / radius, y3 / radius, z3 / radius);
                fig.addNormal(x4 / radius, y4 / radius, z4 / radius);

                fig.addTexture((float)j / slices, (float)i / stacks);
                fig.addTexture((float)j / slices, (float)(i + 1) / stacks);
                fig.addTexture((float)(j + 1) / slices, (float)(i + 1) / stacks);

                int index = (i * slices + j) * 6;
                fig.addTriangle(index, index + 1, index + 2);
            }

            if (i != stacks - 1) {
                fig.addVertex(x1, y1, z1);
                fig.addVertex(x4, y4, z4);
                fig.addVertex(x2, y2, z2);

                fig.addNormal(x1 / radius, y1 / radius, z1 / radius);
                fig.addNormal(x4 / radius, y4 / radius, z4 / radius);
                fig.addNormal(x2 / radius, y2 / radius, z2 / radius);

                fig.addTexture((float)j / slices, (float)i / stacks);
                fig.addTexture((float)(j + 1) / slices, (float)(i + 1) / stacks);
                fig.addTexture((float)(j + 1) / slices, (float)i / stacks);

                int index = (i * slices + j) * 6 + 3;
                fig.addTriangle(index, index + 1, index + 2);
            }
        }
    }
}