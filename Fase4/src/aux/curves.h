#ifndef CURVES_H
#define CURVES_H

#pragma once

#include <math.h>
#include <cmath>
#include <vector>
#include "point.h"

float bernstein(int i, int n, float t);

float bernstein_deriv(int i, float t);

void bezier_curve(int tesselation, float points[4][3], float * result);

void calculate_normal(float *u, float * v, float * output);

void catmullrom_curve(int tessellation, std::vector<Vector3>& points, float* result, float* result_deriv);

void generate_catmull_matrix(float *div,float *y,float *m);

#endif