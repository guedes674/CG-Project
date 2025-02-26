#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "classes/figure.h"

int main(int argc, char* argv[]) {
	if (argc >= 5) {
		Figure fig;
		char* file_path;
		if (strcmp(argv[1], "plane") == 0) {

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