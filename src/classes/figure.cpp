#include "figure.hpp"

class Figure {
private:
	List points;

public:
	void addPoint(Point p) {
		if (p != NULL) {
			points.add(p);
		}
	}

	List getPoints() {
		return points;
	}

	void figureToFile(char* path) {
		if (points.size() == 0) {
			printf("Figure is NULL\n");
			return;
		}
		FILE* file = fopen(path, "w");
		if (!file) {
			printf("Error opening file\n");
			return;
		}
		fprintf(file, "%ld\n", points.size());
		for (long i = 0; i < points.size(); i++) {
			Point p = (Point)points.get(i);
			fprintf(file, "%g,%g,%g\n", getX(p), getY(p), getZ(p));
		}
		fclose(file);
	}

	Figure figureFromFile(char* path) {
		FILE* file = fopen(path, "r");
		if (!file) {
			printf("Error opening file\n");
			return NULL;
		}
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
			addPoint(newPoint(x, y, z));
		}
		fclose(file);
		return this;
	}

	void deleteFigure() {
		if (points.size() > 0) {
			points.deepDeleteList();
		}
	}
};