#ifndef FIGURE
#define FIGURE
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include "list.hpp"
#include "point.hpp"

typedef struct figure* Figure;

Figure newEmptyFigure();

Figure newFigure(List points);

void addPoint(Figure f, Point p);

List getPoints(Figure f);

void figureToFile(Figure f, char* path);

Figure figureFromFile(char* path);

void deleteFigure(void* figure);

#endif