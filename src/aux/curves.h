#ifndef CURVES_H
#define CURVES_H

#pragma once

#include <math.h>
#include <cmath>

float bernstein(int i, int n, float t);

float bernstein_deriv(int i, int n, float t);

void bezier_curve(int tesselation, float points[4][3], float * result);

#endif