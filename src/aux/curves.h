#ifndef CURVES_H
#define CURVES_H

#pragma once

#include <math.h>
#include <cmath>
#include <vector>
#include "point.h"

float bernstein(int i, int n, float t);

float bernstein_deriv(int i, int n, float t);

void bezier_curve(int tesselation, float points[4][3], float * result);

void catmullrom_curve(int tessellation, std::vector<point>& points, float* result);

#endif