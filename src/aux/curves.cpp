#include "curves.h"

float bernstein(int i, int n, float t) {
    // Precomputed binomial coefficients for cubic Bézier (n=3)
    (void)n;
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

void catmullrom_curve(int tessellation, std::vector<Vector3>& points, float* result, float* result_deriv) {
    const int n = points.size();
    const float step = 1.0f / tessellation;
    int offset = 0;

    // Catmull-Rom basis matrix
    float M[4][4] = {
        {-0.5f,  1.5f, -1.5f,  0.5f},
        { 1.0f, -2.5f,  2.0f, -0.5f},
        {-0.5f,  0.0f,  0.5f,  0.0f},
        { 0.0f,  1.0f,  0.0f,  0.0f}
    };

    for (int i = 0; i < n; ++i) {
        // P0, P1, P2, P3 com wrap-around
        Vector3 P0 = points[(i - 1 + n) % n];
        Vector3 P1 = points[i];
        Vector3 P2 = points[(i + 1) % n];
        Vector3 P3 = points[(i + 2) % n];

        for (float t = 0.0f; t <= 1.0f + 1e-6f; t += step) {
            float T[4] = {t * t * t, t * t, t, 1.0f};
            float Td[4] = {3 * t * t, 2 * t, 1.0f, 0.0f};

            for (int axis = 0; axis < 3; ++axis) {
                float P[4] = {
                    axis == 0 ? P0.x : (axis == 1 ? P0.y : P0.z),
                    axis == 0 ? P1.x : (axis == 1 ? P1.y : P1.z),
                    axis == 0 ? P2.x : (axis == 1 ? P2.y : P2.z),
                    axis == 0 ? P3.x : (axis == 1 ? P3.y : P3.z)
                };

                float A[4] = {0, 0, 0, 0};
                for (int j = 0; j < 4; ++j) {
                    for (int k = 0; k < 4; ++k) {
                        A[j] += M[j][k] * P[k];
                    }
                }

                float pos = 0.0f;
                float deriv = 0.0f;
                for (int j = 0; j < 4; ++j) {
                    pos += T[j] * A[j];
                    deriv += Td[j] * A[j];
                }

                result[offset] = pos;
                result_deriv[offset] = deriv;
                ++offset;
            }
        }
    }
}

void build_rot_matrix(float *x, float *y, float *z, float *m) {
	m[0] = x[0]; m[1] = x[1]; m[2] = x[2]; m[3] = 0;
	m[4] = y[0]; m[5] = y[1]; m[6] = y[2]; m[7] = 0;
	m[8] = z[0]; m[9] = z[1]; m[10] = z[2]; m[11] = 0;
	m[12] = 0; m[13] = 0; m[14] = 0; m[15] = 1;
}


void cross(float *a, float *b, float *res) {
	res[0] = a[1]*b[2] - a[2]*b[1];
	res[1] = a[2]*b[0] - a[0]*b[2];
	res[2] = a[0]*b[1] - a[1]*b[0];
}


void normalize(float *a) {
	float l = sqrt(a[0]*a[0] + a[1] * a[1] + a[2] * a[2]);
	a[0] = a[0]/l;
	a[1] = a[1]/l;
	a[2] = a[2]/l;
}

void generate_catmull_matrix(float *div,float *y,float *m){

    float z[4];

    normalize(div);
    cross(div,y,z);
    normalize(z);
    cross(z,div,y);
    normalize(y);

    build_rot_matrix(div,y,z,m);
}