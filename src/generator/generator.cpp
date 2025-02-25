#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "classes/figure.h"

Figure plane(int lenght, int divisions) {
	Figure fig;
	fig = Figure();
	fig.addVertex(0, 0, 0);
	fig.addVertex(lenght, 0, 0);
	fig.addVertex(0, 0, lenght);
	fig.addVertex(lenght, 0, lenght);
	fig.addNormal(0, 1, 0);
	fig.addNormal(0, 1, 0);
	fig.addNormal(0, 1, 0);
	fig.addNormal(0, 1, 0);
	fig.addTexture(0, 0);
	fig.addTexture(1, 0);
	fig.addTexture(0, 1);
	fig.addTexture(1, 1);
	fig.addTriangle(0, 1, 2);
	fig.addTriangle(1, 3, 2);
	figureToFile(fig,"plane.3d");
	return fig;
}

int main(int argc, char* argv[]) {
	if (argc >= 5) {
		Figure fig;
		char* file_path;
		if (strcmp(argv[1], "plane") == 0) {
			int lenght = atoi(argv[2]);
			int divisions = atoi(argv[3]);
			file_path = argv[4];
			fig = plane(lenght, divisions);

		}
		else if (strcmp(argv[1], "box") == 0) {

		}
		else if (strcmp(argv[1], "sphere") == 0) {
		}
		else if (strcmp(argv[1], "cone") == 0) {
		}
		else {
			printf("Figura inválida\n");
			return 1;
		}
	}
	else{
		printf("Número de argumentos inválido\n");
		return 1;
	}
}