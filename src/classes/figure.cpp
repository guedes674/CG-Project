#include "figure.hpp"

struct figure {
	List points;
};

Figure newEmptyFigure() {
	Figure f = (Figure)malloc(sizeof(struct figure));
	if (f != NULL) {
		f->points = newEmptyList();
	}
	return f;
}

Figure newFigure(List points) {
	Figure f = (Figure)malloc(sizeof(struct figure));
	if (f != NULL) {
		for (long i = 0; i < size(points); i++) {
			addPoint(f, (Point)get(points, i));
		}
	}
	return f;
}

void addPoint(Figure f, Point p) {
	if (f != NULL) {
		add(f->points, p);
	}
}

List getPoints(Figure f) {
	if (f) {
		return f->points;
	}
	return NULL;
}

void figureToFile(Figure f, char* path) {
	if (!f) {
		printf("Figure is NULL\n");
		return;
	}
	FILE* file = fopen(path, "w");
	if (!file) {
		printf("Error opening file\n");
		return;
	}
	for (long i = 0; i < size(f->points); i++) {
		Point p = (Point)get(f->points, i);
		fprintf(file, "%g,%g,%g\n", getX(p), getY(p), getZ(p);
	}
	fclose(file);
}

Figure figureFromFile(char* path) {
	Figure f = newEmptyFigure();
	FILE* file = fopen(path, "r");
	if (f && file) {
		char buffer[1024];
		fgets(buffer, 1023, file);
		int vertexs = atoi(buffer);
		for (int i = 0; i < vertexs; i++) {
			fgets(buffer, 1023, file);
			char* token = strtok(buffer, ",");
			double x = atof(token);
			token = strtok(NULL, ",");
			double y = atof(token);
			token = strtok(NULL, ",");
			double z = atof(token);
			addPoint(f, newPoint(x, y, z));
		}
		fclose(file);
	}
	return f;
}

void deleteFigure(void* figure) {
	if (f != NULL) {
		for (long i = 0; i < size(((Figure)f)->points); i++) {
			deletePoint(get((Figure)f)->points, i);
		}
		deleteList(f->points);
		free(f);
	}
}