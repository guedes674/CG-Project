#include "curves.h"

// Bernstein basis function for Bézier curves
float bernstein(int i, int n, float t) {
    // Precomputed binomial coefficients for cubic Bézier (n=3)
    const float coeffs[4] = {1.0f, 3.0f, 3.0f, 1.0f};
    return coeffs[i] * std::pow(t, i) * std::pow(1 - t, n - i);
}

float bernstein_deriv(int i, int n, float t) {
    return n * (bernstein(i-1, n-1, t) - bernstein(i, n-1, t));
}

void bezier_curve(int tessellation, float points[4][3], float* result) {
    const float step = 1.0f / tessellation;
    int offset = 0;

    // Iterate through parameter t from 0 to 1
    for (float t = 0.0f; t <= 1.0f + 1e-6; t += step) {  // +1e-6 to account for fp errors
        // Calculate Bernstein basis weights
        float B[4];
        for (int i = 0; i < 4; ++i) {
            B[i] = bernstein(i, 3, t);
        }

        // Calculate position for each axis (X, Y, Z)
        for (int axis = 0; axis < 3; ++axis) {
            float sum = 0.0f;
            for (int i = 0; i < 4; ++i) {
                sum += points[i][axis] * B[i];
            }
            result[offset++] = sum;
        }
    }
}