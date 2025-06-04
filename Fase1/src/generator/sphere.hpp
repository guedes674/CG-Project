#ifndef SPHERE
#define SPHERE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cmath>
#include "../classes/figure.hpp"

// Fun��o que gera uma esfera de acordo com o raio, slices e stacks passados
Figure generateSphere(float radius, int slices, int stacks);

#endif