#include "box.hpp"

Figure box(float length, int divisions) {
    Figure fig;
    fig = Figure();

    float step = length / divisions;
    float half = length / 2.0f;

    for (int face = 0; face < 6; face++) {
        for (int i = 0; i < divisions; i++) {
            for (int j = 0; j < divisions; j++) {
                float x1, y1, z1, x2, y2, z2;

                switch (face) {
                case 0: // frente
                    x1 = -half + i * step;
                    x2 = -half + (i + 1) * step;
                    y1 = -half + j * step;
                    y2 = -half + (j + 1) * step;
                    z1 = half;
                    z2 = half;
                    fig.addNormal(0, 0, 1);
                    break;
                case 1: // atras
                    x1 = -half + (i + 1) * step;
                    x2 = -half + i * step;
                    y1 = -half + j * step;
                    y2 = -half + (j + 1) * step;
                    z1 = -half;
                    z2 = -half;
                    fig.addNormal(0, 0, -1);
                    break;
                case 2: // esquerda
                    x1 = -half;
                    x2 = -half;
                    y1 = -half + j * step;
                    y2 = -half + (j + 1) * step;
                    z1 = -half + i * step;
                    z2 = -half + (i + 1) * step;
                    fig.addNormal(-1, 0, 0);
                    break;
                case 3: // direita
                    x1 = half;
                    x2 = half;
                    y1 = -half + j * step;
                    y2 = -half + (j + 1) * step;
                    z1 = -half + (i + 1) * step;
                    z2 = -half + i * step;
                    fig.addNormal(1, 0, 0);
                    break;
                case 4: // cima
                    x1 = -half + i * step;
                    x2 = -half + (i + 1) * step;
                    y1 = half;
                    y2 = half;
                    z1 = -half + j * step;
                    z2 = -half + (j + 1) * step;
                    fig.addNormal(0, 1, 0);
                    break;
                case 5: // baixo
                    x1 = -half + i * step;
                    x2 = -half + (i + 1) * step;
                    y1 = -half;
                    y2 = -half;
                    z1 = -half + (j + 1) * step;
                    z2 = -half + j * step;
                    fig.addNormal(0, -1, 0);
                    break;
                }

                fig.addVertex(x1, y1, z1);
                fig.addVertex(x2, y1, z1);
                fig.addVertex(x1, y2, z2);
                fig.addVertex(x2, y2, z2);

                fig.addTexture(0, 0);
                fig.addTexture(1, 0);
                fig.addTexture(0, 1);
                fig.addTexture(1, 1);

                int index = (face * divisions * divisions + i * divisions + j) * 4;
                fig.addTriangle(index, index + 1, index + 2);
                fig.addTriangle(index + 1, index + 3, index + 2);
            }
        }
    }
}