#include "../include/my_lib.h"
#include <math.h>
#include <float.h>

Float Derivative(float A, float deltaX) {
    float dx = fabsf(deltaX);

    if (dx == 0.0f || dx < FLT_EPSILON) {
        return (Float)NAN;
    }

    float f_plus = cosf(A + dx);
    float f_minus = cosf(A - dx);

    return (Float)((f_plus - f_minus) / (2.0f * dx));
}

Int GCF(int A, int B) {
    int gcd = 1;
    int limit = (A < B) ? A : B;

    for (int i = 1; i <= limit; i++) {
        if (A % i == 0 && B % i == 0) {
            gcd = i;
        }
    }
    return gcd;
}