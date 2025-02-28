#include "generator.hpp"

int main(int argc, char* argv[]) {
    if (argc >= 5) {
        Figure fig;
        char* file_path = argv[4];

        if (strcmp(argv[1], "plane") == 0) {
            int length = atoi(argv[2]);
            int divisions = atoi(argv[3]);
			file_path = argv[4];
            fig = plane(length, divisions);
        }
        else if (strcmp(argv[1], "box") == 0) {
            float length = atof(argv[2]);
            int divisions = atoi(argv[3]);
            fig = box(length, divisions);
        }
        else if (strcmp(argv[1], "sphere") == 0) {
            float radius = atof(argv[2]);
            int slices = atoi(argv[3]);
            int stacks = atoi(argv[4]);
            fig = generateSphere(radius, slices, stacks);
        }
        else if (strcmp(argv[1], "cone") == 0) {
        }
        else {
            printf("Figura inválida\n");
            return 1;
        }

        fig.figureToFile(file_path);
        printf("Arquivo %s gerado com sucesso!\n", file_path);
    }
    else {
        printf("Número de argumentos inválido\n");
        return 1;
    }

    return 0;
}