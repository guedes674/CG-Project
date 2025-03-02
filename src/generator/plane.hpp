#ifndef PLANE
#define PLANE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../classes/figure.hpp"

// Função que gera um plano de acordo com o tamanho e divisões passadas
Figure generatePlaneXZ(float length, int divisions, int height);

Figure generatePlaneXY(float length, int divisions, int height);

Figure generatePlaneYZ(float length, int divisions, int height);
#endif