#include "curves.h"

float bernstein(int i, int n, float t) {
    // Precomputed binomial coefficients for cubic Bézier (n=3)
    static constexpr float coeffs[4] = {1.0f, 3.0f, 3.0f, 1.0f};
    const float mt = 1.0f - t;  // Reuse (1 - t) term

    // Compute t^i and (1-t)^(3-i) using explicit multiplication (no std::pow)
    switch (i) {
        case 0: return coeffs[i] * (mt * mt * mt);          // t^0 * (1-t)^3
        case 1: return coeffs[i] * (t * mt * mt);            // t^1 * (1-t)^2
        case 2: return coeffs[i] * (t * t * mt);             // t^2 * (1-t)^1
        case 3: return coeffs[i] * (t * t * t);              // t^3 * (1-t)^0
        default: return 0.0f;  // Invalid index (safety guard)
    }
}

float bernstein_deriv(int i, int n, float t) {
    return n * (bernstein(i-1, n-1, t) - bernstein(i, n-1, t));
}

void bezier_curve(int tessellation, float points[4][3], float* result) {
    const float step = 1.0f / tessellation;
    int offset = 0;

    for (float t = 0.0f; t <= 1.0f + 1e-6; t += step) {
        // Precompute (1-t) and its powers
        const float mt = 1.0f - t;
        const float mt2 = mt * mt;
        const float mt3 = mt2 * mt;
        const float t2 = t * t;
        const float t3 = t2 * t;

        // Hardcoded Bernstein weights for cubic Bézier
        const float B[4] = {
            1.0f * mt3,          // i=0
            3.0f * t * mt2,      // i=1
            3.0f * t2 * mt,      // i=2
            1.0f * t3            // i=3
        };

        // Compute position (same as before)
        for (int axis = 0; axis < 3; ++axis) {
            float sum = 0.0f;
            for (int i = 0; i < 4; ++i) {
                sum += points[i][axis] * B[i];
            }
            result[offset++] = sum;
        }
    }
}

void catmull_rom_curve(int tessellation, float points[4][3], float* result) {
    const float step = 1.0f / tessellation;
    int offset = 0;

    // Catmull-Rom base matrix
    float M[4][4] = {
        {-0.5f,  1.5f, -1.5f,  0.5f},
        { 1.0f, -2.5f,  2.0f, -0.5f},
        {-0.5f,  0.0f,  0.5f,  0.0f},
        { 0.0f,  1.0f,  0.0f,  0.0f}
    };

    for (float t = 0.0f; t <= 1.0f + 1e-6f; t += step) {
        float T[4] = {t * t * t, t * t, t, 1.0f};

        for (int axis = 0; axis < 3; ++axis) {
            float P[4] = {
                points[0][axis],
                points[1][axis],
                points[2][axis],
                points[3][axis]
            };

            // Multiply M * P
            float A[4] = {0, 0, 0, 0};
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    A[i] += M[i][j] * P[j];
                }
            }

            // Multiply T * A
            float value = 0.0f;
            for (int i = 0; i < 4; ++i) {
                value += T[i] * A[i];
            }

            result[offset++] = value;
        }
    }
}
