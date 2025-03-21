#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <utility>
#include "../classes/figure.hpp"
#include "plane.hpp"
#include "box.hpp"
#include "sphere.hpp"

int main(int argc, char* argv[]) {
    if (argc >= 6) {
        char* file_path = argv[5];

        if (strcmp(argv[1], "plane") == 0) {
            int length = atoi(argv[2]);
            int divisions = atoi(argv[3]);
            Figure fig = generatePlaneXZ(length, divisions, 0);
            fig.figureToFile(file_path);
        }
        else if (strcmp(argv[1], "box") == 0) {
            float length = atof(argv[2]);
            int divisions = atoi(argv[3]);
            Figure fig = generateBox(length, divisions);
            fig.figureToFile(file_path);
        }
        else if (strcmp(argv[1], "sphere") == 0) {
            float radius = atof(argv[2]);
            int slices = atoi(argv[3]);
            int stacks = atoi(argv[4]);
            Figure fig = generateSphere(radius, slices, stacks);
            fig.figureToFile(file_path);
        }
        else if (strcmp(argv[1], "cone") == 0) {
        }
        else {
            printf("Figura inválida\n");
            return 1;
        }
        printf("Arquivo %s gerado com sucesso!\n", file_path);
    }
    else {
        printf("Número de argumentos inválido\n");
        return 1;
    }

    return 0;
}